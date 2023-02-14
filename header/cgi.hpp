#ifndef CGI_HPP
#define CGI_HPP
#include "header.hpp"

class Cgi
{
    public :
        Cgi();
        Cgi(std::string path, const ConfigParsing *conf, Request *req, std::string cgiPath);
        Cgi(const Cgi &other);
        ~Cgi();
        std::string     _header;
        std::string     _data;
        std::string     _cgiType;
        size_t          _code;
    private :
        std::string     _cgiPath;
        std::string     _filename;
        const ConfigParsing   *_config;
        std::string     _path;
        Request         *_request;
        sockaddr_in     _address;
};

#endif
