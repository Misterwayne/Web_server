/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_handling.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mwane <mwane@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/15 19:56:18 by mwane             #+#    #+#             */
/*   Updated: 2022/10/12 17:53:49 by mwane            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HANDLING_HPP
#define HANDLING_HPP

#include "header.hpp"
#include "request.hpp"

class Request;
class ConfigParsing;
class Cgi;

class Response
{
    private:
        std::string                         _method;
        std::string                         _return_body;
        Request                             *_request;
        std::map<int, std::string>          _statusCode;
        std::map<std::string, std::string>  _header;
        std::string                         _response;
        ConfigParsing   					*_config;
        ConfigParsing                       *_location;
        size_t                              _code;
        bool                                _delete_on;
        std::string                         _return_header;
        std::string                         _body;
        std::string                         _path;
        std::map<std::string, std::string>  _contentType;
        std::string                         _cgiType;
        std::string                         _cgiPath;
        bool                                _isLogged;
        bool                                _islogout;
    public:

        Response ();

        Response(Request *request, ConfigParsing *config);
        ~Response();

        void            check_size();
        void            check_path();
        void            error_page(size_t code);
        void            initStatusCode();
        void            getMethod();
        std::string     post_page();
        int             get_page(std::string path);
        void            set_header();
        const char      *getResponse();
        void            get_dir(std::string path);
        void	        getContentType();
        void            delete_page();
        bool            create_file_session(std::string body);
        std::string     search_file_session(std::string body);
        
        size_t                              _size;
        bool                                _ready;
};

#endif