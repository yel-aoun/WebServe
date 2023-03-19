#include "Post.hpp"
#include "../server/server.hpp"
// void send_chunk(int sockfd, const char* data, int size) {
//     // Convert the size to hexadecimal format
//     stringstream ss;
//     ss << hex << size << "\r\n";
//     string size_str = ss.str();

//     // Send the chunk size and data to the server
//     send(sockfd, size_str.c_str(), size_str.length(), 0);
//     send(sockfd, data, size, 0);
//     send(sockfd, "\r\n", 2, 0);
// }

std::string Post::check_hexa(std::string buff)
{
    std::string hex;
    int i = 0;
    int ind = buff.find("\r\n");
    
    std::cout << "INDEX ======== " << ind << std::endl;
    while (buff[ind]!= '\r' && buff[ind]!= '\n')
        hex += buff[ind++];
    
    return (hex);
}


int Post::hexToDec(const std::string& hexStr) {
    std::stringstream ss;
    ss << std::hex << hexStr;
    int decNum;
    ss >> decNum;
    return decNum;
}

std::string Post::seperate_header(std::string buff)
{
    if (body_or_head == 1)
        return (buff);
    if (buff.find("\r\n\r\n") == -1)
        return (buff);
    int x = buff.find("\r\n\r\n") + 2;
    std::string body = buff.substr(x, buff.size() - (x + 1));
    body_or_head = 1;
    return (body);
}

int Post::skip_hex(std::string body)
{
     int i = body.find("\r\n") + 1;
    while (body[++i])
        if (body[i] == '\n')
            break;
    return (i + 1);
}

void Post::exec_head(std::string buff, Server serv, std::string &path)
{
    std::cout << "Hello" << std::endl;
    if (!path.empty())
    {
        std::list<location> loc = serv.get_locations();
        for (std::list<location>::iterator it = loc.begin(); it != loc.end(); ++it)
        {
            if (it->get_locations() == path)
            {
                if (access(it->get_upload_pass().c_str(), F_OK))
                    int status = mkdir(it->get_upload_pass().c_str(), 0777);
                std::ofstream file(it->get_upload_pass() + "/testing");
                if (file.is_open())
                {
                    std::string body = seperate_header(buff);
                    std::string hex = check_hexa(body);
                    int num_to_read = hexToDec(hex);
                     int ind = skip_hex(body);
                    file.write(&body[ind], num_to_read);
                    //if (i == body.find("\r\n0\r\n"))
                    //{
                        file.close();
                      //  break;
                    //}
                }
            }
        }
    }
}

void Post::exec_body(std::string buff, Server serv, std::string &path)
{
    std::cout << "Hello22222222222222222" << std::endl;
    if (!path.empty())
    {
        std::list<location> loc = serv.get_locations();
        for (std::list<location>::iterator it = loc.begin(); it != loc.end(); ++it)
        {
            if (it->get_locations() == path)
            {
                if (access(it->get_upload_pass().c_str(), F_OK))
                    int status = mkdir(it->get_upload_pass().c_str(), 0777);
                std::ofstream file(it->get_upload_pass() + "/testing");
                if (file.is_open())
                {
                    std::string hex = check_hexa(buff);
                    std::cout << "HEX ===== " << hex << std::endl;
                    int num_to_read = hexToDec(hex);
                     int ind = skip_hex(buff);
                    file.write(&buff[ind], num_to_read);
                    //if (i == body.find("\r\n0\r\n"))
                    //{
                        file.close();
                      //  break;
                    //}
                }
            }
        }
    }
 //   std::ofstream outfile("test2.txt"); 

    // if (outfile.is_open())
    // {
        
    // }
    // ifstream file("myfile.txt");
    // if (!file.is_open()) {
    //     cerr << "Error opening file." << endl;
    //     return 1;
    // }
    // char buffer[1024];
    // while (!file.eof()) {
    //     file.read(buffer, sizeof(buffer));
    //     int size = file.gcount();
    //     send_chunk(sockfd, buffer, size);
    //}

}