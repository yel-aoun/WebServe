#ifndef POST_HPP
#define POST_HPP

#include "../headers_cpp.hpp"
class Server;

class Post
{
    private:
        int body_or_head;
        int _post_type;
        std::string boundry;
        std::string _buff;
        std::map<std::string, std::string> _extensions;
        void (*_post_func[3])();
    public:
        Post();
        std::string generate_file_name(std::string extension);
        void        generate_extensions(void);
        void        normal_post(std::string buff, Server &server);
        void        exec_head(std::string buff, Server &serv, std::string &path);
        void        exec_body(std::string buff, Server &serv, std::string &path);
        int         skip_hex(std::string body);
        std::string seperate_header(std::string buff);
        int         hexToDec(const std::string& hexStr);
        std::string check_hexa(std::string buff);
        ~Post(){}
};
#endif