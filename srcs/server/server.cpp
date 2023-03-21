#include "server.hpp"
#include "../parsing/location.hpp"
#include "../parsing/parce_server.hpp"
#include "socket.hpp"
#include "client.hpp"

Server::Server(parce_server &server_data)
{
    this->_port = server_data.port;
    this->_host_name = server_data.host_name;
    this->_max_client_body_size = server_data.max_client_body_size;
    this->_error_page = server_data.error_page;
    this->_locations = server_data.locations;
    Socket socket(this->_port);
    this->_server_socket = socket.get_socket();
}

std::list<location> Server::get_locations() const
{
    return(this->_locations);
};

void  Server::init_sockfds()
{
    FD_ZERO(&this->_reads);
    FD_ZERO(&this->_writes);
    FD_SET(this->_server_socket, &this->_reads);
    this->_max_socket = this->_server_socket;
    std::list<Client *>::iterator iter;
    for(iter = this->_clients.begin(); iter != this->_clients.end(); iter++)    {
        FD_SET((*iter)->get_sockfd(), &this->_writes);
        FD_SET((*iter)->get_sockfd(), &this->_reads);
        if ((*iter)->get_sockfd() > this->_max_socket)
            this->_max_socket = (*iter)->get_sockfd();
    }
}

void    Server::wait_on_clients()
{
    struct timeval restrict;

    this->init_sockfds();
    restrict.tv_sec = 1;
    restrict.tv_usec = 0;
    if (select(this->_max_socket + 1, &(this->_reads), NULL, NULL, &restrict) < 0)
    {
        std::cerr << "select() failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

const char *get_client_address(Client *ci)
{
    static char address_buffer[100];
    getnameinfo((struct sockaddr*)&ci->_address,
    ci->_address_length,
    address_buffer, sizeof(address_buffer), 0, 0,
    NI_NUMERICHOST);
    return address_buffer;
}

void    Server::accept_new_client()
{
    Client *client = new Client;

    SOCKET r = accept(this->_server_socket, \
        reinterpret_cast<struct sockaddr *>(&client->_address), \
        &(client->_address_length));
    client->set_sockfd(r);
    if (!ISVALIDSOCKET(client->get_sockfd()))
    {
        std::cerr << "accept() failed." << std::endl;
        exit(EXIT_FAILURE);
    }
    this->_clients.push_back(client);
}

void    Server::run_serve()
{
    this->wait_on_clients();
    if (FD_ISSET(this->_server_socket, &this->_reads))
        this->accept_new_client();
    else
        this->serve_clients();
}

void    Server::serve_clients()
{
    int                             request_size;
    std::list<Client *>::iterator   iter;
    for(iter = this->_clients.begin(); iter != this->_clients.end(); iter++)
    {
        if(FD_ISSET((*iter)->get_sockfd(), &this->_reads))
        {
            memset(this->_request_buff, 0, MAX_REQUEST_SIZE + 1);
            request_size = recv((*iter)->get_sockfd(), this->_request_buff, MAX_REQUEST_SIZE, 0);
            //std::cout << request_size << std::endl;
            if (request_size < 1)
            {
                std::cerr << "Unexpected disconnect from << " << get_client_address(*iter) << std::endl;
                drop_client(iter);
                continue;
            }
            (*iter)->set_received_data(request_size);
            if(!(*iter)->_request_type)
            {
                (*iter)->_request.append(this->_request_buff);
                if(std::strstr((*iter)->_request.c_str() , "\r\n\r\n"))
                {
                    Request req((*iter)->_request, iter);
                    if(req.method == "POST")
                    {
                        check_path((*iter)->path, (*iter)->request_pack, (*iter)->content_type, iter);
                        (*iter)->init_post_data();
                        (*iter)->_request_type = true;
                        std::strcpy(this->_request_buff, seperate_header((*iter)->_request).c_str());
                        (*iter)->post.call_post_func(*this, *iter);
                    }
                }
            }
            else // this else is for just post becouse post containe the body.
            {
                (*iter)->post.call_post_func(*this, *iter);
            }
        }
        else
        {
            if((*iter)->method == "POST")
                (*iter)->file.close();
            exit(0);
        }
    }
}

void    Server::drop_client(std::list<Client *>::iterator client)
{
    CLOSESOCKET((*client)->get_sockfd());
    std::list<Client *>::iterator iter;

    for(iter = this->_clients.begin(); iter != this->_clients.end(); iter++)
    {
        if((*client)->get_sockfd() == (*iter)->get_sockfd())
            iter = this->_clients.erase(iter);
        return ;
    }
    std::cerr << "Drop Client not found !" << std::endl;
}

Server::~Server()
{
    // close server socket;
}

void    Server::check_path(std::string &path, std::map<std::string, std::vector<std::string> > &map_req, int &content_type, std::list<Client *>::iterator iter)
{
    check_transfer_in_coding(map_req, content_type, iter);
}

void    Server::check_transfer_in_coding(std::map<std::string, std::vector<std::string> > &map_req, int &content_type, std::list<Client *>::iterator iter)
{
    std::map<std::string, std::vector<std::string> >::iterator m_ap = map_req.find("Transfer-Encoding");
    if (m_ap != map_req.end())
    {
        std::vector<std::string> vec = m_ap->second;
        std::vector<std::string>::iterator itt = vec.begin();
        for(; itt != vec.end(); itt++)
        {
            if ((*itt) != "chunked")
            {
                std::cout<< "error /501"<<std::endl;
                drop_client(iter);
                return ;
            }
            else
            {
                if (content_type == 1)
                {
                    std::cout<<"error not implemented"<<std::endl;
                    exit(0);
                }
                else
                    content_type = 2;
            }
        }
    }
    return ;
}

std::string Server::seperate_header(std::string buff)
{
    int x = buff.find("\r\n\r\n") + 4;
    std::string body = buff.substr(x, buff.size() - (x + 1));
    return (body.c_str());
}