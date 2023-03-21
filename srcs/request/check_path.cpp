# include "check_path.hpp"

Check_path::Check_path(std::list<Client *>::iterator iter): skip(0)
{
    check_transfer_encoding(iter);
}

void   Check_path::check_transfer_encoding(std::list<Client *>::iterator iter)
{
    // (*iter)->path, (*iter)->request_pack, (*iter)->content_type,
    std::map<std::string, std::vector<std::string> > map_req = (*iter)->request_pack;
    std::map<std::string, std::vector<std::string> >::iterator m_ap = map_req.find("Transfer-Encoding");
    if (m_ap != map_req.end())
    {
        std::vector<std::string> vec = m_ap->second;
        std::vector<std::string>::iterator itt = vec.begin();
        if (itt == vec.end())
        {
            std::cout<< "Transfer-Encoding with empty value empty error /501"<<std::endl;
            // drop_client(iter);
            return ;
        }
        else
        {
            for(; itt != vec.end(); itt++)
            {
                if ((*itt) != "chunked")
                {
                    std::cout<< "Transfer-Encoding not a match error /501 not implemented"<<std::endl;
                    // drop_client(iter);
                    std::cout<<"heeeer"<<std::endl;
                    this->skip = 1;
                    return ;
                }
                else
                {
                    if ((*iter)->content_type == 1)
                    {
                        std::cout<<"Transfer-Encoding(chunked) and boundry error not implemented"<<std::endl;
                        // std::cout<< "error /501"<<std::endl;
                        // drop_client(iter);
                        this->skip = 1;
                        //set_error page of not implemented
                        return ;
                    }
                    else
                        (*iter)->content_type = 2;
                }
            }
        }
    }
    // std::cout<<"heeer : "<<(*iter)->content_type<<std::endl;
    if (((*iter)->content_type == 0 || (*iter)->content_type == 1) && ((*iter)->method == "POST"))
    {
        std::map<std::string, std::vector<std::string> > map_req = (*iter)->request_pack;
        std::map<std::string, std::vector<std::string> >::iterator m_ap = map_req.find("Content-Length");
        if (m_ap != map_req.end())
        {
            std::vector<std::string> vec = m_ap->second;
            std::vector<std::string>::iterator itt = vec.begin();
            if (itt == vec.end())
            {
                std::cout<< "Content-Length exist with no value error /400 bad request"<<std::endl;
                // drop_client(iter);
                this->skip = 1;
                return ;
            }
        }
        else
        {
            std::cout<< "Content-Length not exist error /400 bad request"<<std::endl;
            // drop_client(iter);
            this->skip = 1;
            return ;
        }
    }
    check_uri(iter);
    return ;
}

bool isCharAllowed(char c) {
    // List of allowed characters
    const std::string allowedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
    
    // Check if the character is in the allowed list
    return (allowedChars.find(c) != std::string::npos);
}

bool isURIValid(const std::string& uri, int len) {
    // Check each character in the URI
    for (int i = 0; i < len; i++) {
        if (!isCharAllowed(uri[i])) {
            // Character not allowed, URI is invalid
            return false;
        }
    }
    
    // All characters are allowed, URI is valid
    return true;
}

void    Check_path::check_uri(std::list<Client *>::iterator iter)
{
    std::string uri = (*iter)->path;
    int len = uri.length();
    if (len > 2048)
    {
        std::cout<<"request-URI- too long error /414"<<std::endl;
        // drop-client;
        this->skip = 1;
        return ;
    }
    if (!isURIValid(uri, len)) {
        std::cout << "URI have invalid characters error /400 bad request" << std::endl;
        // drop-client;
        this->skip = 1;
        return ;
    }
    // check for request body if larger than client max body size
    // get_matched_location_for_request_uri();
}