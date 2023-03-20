# include "request.hpp"

Request::Request(std::string &buffer, std::list<Client *>::iterator   iter): content_type(0)
{
    std::stringstream ss(buffer);
    std::string token;
    int i = 0;
    while (std::getline(ss, token))
    {
        fill_map_request(token);
        i++;
    }
    (*iter)->request_pack = this->request;
    (*iter)->method = this->method;
    (*iter)->path = this->path;
    (*iter)->query = this->query;
    (*iter)->http = this->http;
    (*iter)->content_type = this->content_type;
    (*iter)->boundary = this->boundary;
}

void    Request::fill_map_request(std::string   &buff_line)
{
    std::stringstream ss(buff_line);
    std::string token;
    ss >> token;
    if (token == "GET" || token == "POST" || token == "DELETE")
    {
        this->method = token;
        ss >> token;
        std::vector<std::string> path_query;
        std::stringstream str(token);
        std::string splt;
        while (getline(str, splt, '?'))
            path_query.push_back(splt);
        std::vector<std::string>::iterator iter = path_query.begin();
        std::string find = from_hexa_to_decimal(*iter);
        this->path = find;
        if (++iter  != path_query.end())
            this->query = *iter;
        ss >> token;
        this->http = token;
    }
   else
   {
        std::vector<std::string> value;
        int len = token.length();
        std::string key = token.substr(0, len - 1);
        if (key == "Content-Type")
        {
            ss >> token;
            ss >> token;
            this->content_type = 1;
            this->boundary = &token[9];
            std::cout<<this->boundary<<std::endl;
        }
        else
        {
            while (ss >> token)
                value.push_back(token);
        }
        this->request.insert(std::make_pair(key,value));
   }
}

std::string Request::from_hexa_to_decimal(std::string &str)
{
    std::string path;
    if (str.rfind('%') == std::string::npos)
        return (str);
    else
    {
        std::string::iterator it = str.begin();
        for(; it != str.end(); it++)
        {
            if (*it == '%' && (it + 1) != str.end() && (it + 2) != str.end() && *(it + 1) != '%' && *(it + 2) != '%')
            {
                it++;
                std::string tmp;
                tmp += *it;
                tmp += + *(it + 1);
                path += get_decimal(tmp);
                it++;
            }
            else
                path += *it;
        }
    }
    return (path);
}

int Request::get_decimal(std::string tmp)
{
    std::istringstream iss(tmp);
    int hexa_val;
    iss>>std::hex>>hexa_val;
    return (hexa_val);
}