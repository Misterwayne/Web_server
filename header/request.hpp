#ifndef _REQUEST_HPP
# define _REQUEST_HPP

#include "header.hpp"

class Request
{
    private:
        std::string                         _method;
        std::string                         _query;
        std::string                         _path;
        std::string                         _version;
        std::map<std::string, std::string>  _headers;
        std::string                         _body;
        size_t                              _statusCode;
        std::string                         _rawString;
        std::vector<std::string>            _methodList;
		std::string							_request;	

    public:
        Request();
        Request(std::string const &str);
        Request(Request const &other);
        ~Request();

        Request &operator=(Request const &rhs);

        void        initHeaders();
        void        initMethodList();
        void        parsing();
		void		clearAll();
        std::string gnl(std::string const &str, size_t &i);
        void        parseFirstLine(std::string const &str);
		
        std::map<std::string, std::string>  getHeaders() const;
        std::string                         getBody() const;
        size_t                              getStatusCode() const;
        std::string                         getRawString() const;
		std::string							getMethod() const;
		std::string							getQuery() const;
		std::string							getPath() const;
		std::string							getVersion() const;
		std::string							getRequest() const;

		void								setStatusCode(size_t statusCode);
		void								setVersion(std::string const &str);
		void								setRequest(char *buff, int size);

		int									ClientSocket;

        class ErrorMethodSpace;
		class ErrorMethod;
		class ErrorPathAndVersion;
		class ErrorVersion;
		class ErrorHttpVersion;

};

class Request::ErrorMethodSpace: public std::exception {
	 public:
        virtual const char * what() const throw() {
            std::cerr << "Error Method Space : no space after method" << std::endl;
            return ("");
        }
};
class Request::ErrorMethod: public std::exception {
	 public:
        virtual const char * what() const throw() {
            std::cerr << "Error Method : method invalid" << std::endl;
            return ("");
        }
};
class Request::ErrorPathAndVersion: public std::exception {
	 public:
        virtual const char * what() const throw() {
            std::cerr << "Error Paths : no path" << std::endl;
            return ("");
        }
};
class Request::ErrorVersion: public std::exception {
	 public:
        virtual const char * what() const throw() {
            std::cerr << "Error Version : no version" << std::endl;
            return ("");
        }
};
class Request::ErrorHttpVersion: public std::exception {
	 public:
        virtual const char * what() const throw() {
            std::cerr << "Error HTTP Version : bad HTTP version" << std::endl;
            return ("");
        }
};


#endif
