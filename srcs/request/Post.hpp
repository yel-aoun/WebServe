#ifndef POST_HPP
#define POST_HPP
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
class Server;
class Post
{
    private:
        int body_or_head;
    public:
        Post() {body_or_head = 0;}
        void exec_head(std::string buff, Server serv, std::string &path);
        void exec_body(std::string buff, Server serv, std::string &path);
        int skip_hex(std::string body);
        std::string seperate_header(std::string buff);
        int hexToDec(const std::string& hexStr);
        std::string check_hexa(std::string buff);
        ~Post(){}
};
#endif