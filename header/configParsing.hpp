#ifndef _CONFIGPARSING_HPP
# define _CONFIGPARSING_HPP

# include "header.hpp"

struct	Listen
{
	size_t port;
	std::string host;
};

class ConfigParsing
{
    public:
        ConfigParsing();
        ConfigParsing(size_t &i, std::vector<std::string> const file);
        ConfigParsing(ConfigParsing const &other);
        ~ConfigParsing();

        ConfigParsing &operator=(ConfigParsing const &other);

        int                                     find_directive(size_t const index, std::vector<std::string> const configFile, std::string *directive, ServerMap dirMap);
        void                                    parsing(size_t &i, std::vector<std::string> const file);
        void                                    skip(size_t &i, std::vector<std::string> const file);

        class	InvalidArgsException : public std::exception 
        {
			virtual const char      *what() const throw();
		};

        Listen				        	        getListen() const;
        std::string                     	    getRoot() const;
        std::vector<std::string>        	    getServerName() const;
        std::map<int, std::string>      	    getErrorPages() const;
        size_t                          	    getClientMaxBodySize() const;
        std::vector<std::string>      		    getAllowedMethods() const;
        std::string				        	    getIndex() const;
        bool                            	    getAutoindex() const;
        bool                            	    getUploadEnable() const;
        std::string                     	    getUploadPath() const;
        std::string                     	    getCgiPath() const;
        std::string                     	    getCgiExtension() const;
        std::string                    		    getAlias() const;		
		std::map<std::string, ConfigParsing>    getLocation() const;
        void                                    setSession(std::string login, std::string password);
        bool                                    checkSession(std::string login);
        void                                    removeSession(std::string login);
        bool                                    searchSession(std::string login, std::string password);
        
        friend	std::ostream &operator<<(std::ostream &out, ConfigParsing const &server);

    private:
        void                        		    init_server_map();
        void                        		    init_location_map();

        void                                    add_listen(std::vector<std::string> args);
        void                                    add_root(std::vector<std::string> args);
        void                                    add_server_name(std::vector<std::string> args);
        void                                    add_error_pages(std::vector<std::string> args);
        void                                    add_client_max_body_size(std::vector<std::string> args);
        void                                    add_allowed_methods(std::vector<std::string> args);
        void                                    add_index(std::vector<std::string> args);
        void                                    add_auto_index(std::vector<std::string> args);
        void                                    add_cgi_path(std::vector<std::string> args);
        void                                    add_cgi_extension(std::vector<std::string> args);
        void                                    add_alias(std::vector<std::string> args);
        void                                    add_upload_enable(std::vector<std::string> args);
        void                                    add_upload_path(std::vector<std::string> args);
        
        Listen				       		        _listen;
        size_t                          	    _clientMaxBodySize;
        std::string                     	    _root;
        std::vector<std::string>        	    _serverName;
        std::map<int, std::string>      	    _errorPages;
		std::vector<std::string>        	    _allowedMethods;
        std::string				        	    _index;
        bool                            	    _autoindex;
        bool                            	    _uploadEnable;
        std::string                     	    _uploadPath;
        std::string                     	    _cgiPath;
        std::string                     	    _cgiExtension;
        std::string                     	    _alias;

        std::map<std::string, ConfigParsing>    _location;
        std::map<std::string, std::string>      _user_session;

        ServerMap                       	    _server_map;
        ServerMap                       	    _location_map;


};

#endif
