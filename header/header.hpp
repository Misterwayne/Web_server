#ifndef HEADER_HPP
# define HEADER_HPP

# define ERROR	-1
# define BACKLOG	655350
# define BUFF	655350
# define TIMEOUT 100
# define ServerMap std::map<std::string, void (ConfigParsing::*)(std::vector<std::string>)>

# define RED	"\033[0;31m"
# define YELLOW	"\033[0;33m"
# define GREEN 	"\033[0;32m"
# define RESET	"\033[0;37m"

# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <cstdlib>
# include <iostream>
# include <unistd.h>
# include <cstring>
# include <fstream>
# include <sstream>
# include <string>
# include <vector>
# include <set>
# include <map>
# include <cstdlib>
# include <poll.h>
# include <fcntl.h>
# include <cerrno>
# include <ctime>
# include <algorithm>

# include <stdio.h>
# include <stdlib.h>
# include <ctype.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <dirent.h>
# include <sys/wait.h>

# include "config.hpp"
# include "request.hpp"
# include "request_handling.hpp"
# include "cgi.hpp"
# include "configParsing.hpp"
# include "config.hpp"
# include "server.hpp"

bool	isNumber(const std::string& s);
bool	isFile(std::string const &path);
bool	isDirectory(std::string const &path);
void	removeDoubleSlash(std::string &str);
void	replacePercent20BySpaces(std::string &str);
bool	ifStringInVector(std::string &str, std::vector<std::string> vec);
void	error_parsing(std::string str);
bool    isHost(std::string &str, std::vector<std::string> vec, size_t port);
std::string create_cmd(std::string str, std::string host, size_t port);
bool check_host(std::string client_host, size_t port, std::string address);

#endif
