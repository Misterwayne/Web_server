#ifndef _SERVER_HPP
# define _SERVER_HPP

#include "header.hpp"

class ConfigParsing;
class Response;
class Request;
class ConfigParsing;
class Config;

class Server
{
	public:
		Server();
		Server(ConfigParsing &conf, Config &allconf);
		Server(Server const &other);
		~Server();

		Server &operator=(Server const &other);

		void	set_server();
		void	routine();

		int						get_socket_server() const;
		bool					getReady();

		class Errorsocket;
		class Errorsetsockopt;
		class Errorbind;
		class Errorlisten;
		class Erroraccept;
		class Errorpoll;

	private:
		int							socket_server;
		std::vector<struct pollfd>	fds;
		std::map<int, Request>		client_buff;
		std::map<int, Response>		response_buff;
		bool						ready;
		ConfigParsing				*config;
		Config						*allConfig;

		void	listen_server();
		void	add_client();
		void	treat_Request(int client);
		int	recv(int socket);
		bool	send(int socket);
		void	close(std::vector<struct pollfd>::iterator it);
		std::vector<ConfigParsing>::iterator	search_server(int socket);
};

class Server::Errorsocket: public std::exception {
	 public:
        virtual const char * what() const throw() {
            return ("Error socket: ");
        }
};

class Server::Errorsetsockopt: public std::exception {
	 public:
        virtual const char * what() const throw() {
            return ("Error setsockopt: ");
        }
};

class Server::Errorbind: public std::exception {
	 public:
        virtual const char * what() const throw() {
            return ("Error bind: ");
        }
};

class Server::Errorlisten: public std::exception {
	 public:
        virtual const char * what() const throw() {
            return ("Error listen: ");
        }
};

class Server::Erroraccept: public std::exception {
	 public:
        virtual const char * what() const throw() {
            return ("Error accept: ");
        }
};

class Server::Errorpoll: public std::exception {
	 public:
        virtual const char * what() const throw() {
            return ("Error poll: ");
        }
};
#endif