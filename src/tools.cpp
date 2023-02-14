#include "../header/header.hpp"
#include "../header/server.hpp"

bool isNumber(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return (!s.empty() && it == s.end());
}

bool isDirectory(std::string const &path) 
{
    struct stat s;
    if (stat(path.c_str(), &s) == 0)
        return S_ISDIR(s.st_mode);
    return false;
}

bool isFile(std::string const &path) 
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
		return S_ISREG(s.st_mode);
	return false;
}

void removeDoubleSlash(std::string &str)
{
	for (size_t i = 0; i < str.size(); i++)
		if (str[i] == '/' && str[i + 1] && str[i + 1] == '/')
			while (str[++i] == '/')
				str.erase(str.begin() + i);
}

bool ifStringInVector(std::string &str, std::vector<std::string> vec)
{
	for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
		if (str == *it)
			return true;
	return false;
}

void	error_parsing(std::string str)
{
    std::cerr << "Error Config File: Unkown Directive \"" << str << "\"." << std::endl;
    throw ConfigParsing::InvalidArgsException();
}

std::string	create_cmd(std::string str, std::string host, size_t port)
{
	(void) port;
	std::string ret = "sudo echo \"" + host + " " + str + "\" >> /etc/hosts";
	return (ret);
}

bool isHost(std::string &str, std::vector<std::string> vec, size_t port)
{
	std::string tmp;
    std::ostringstream tmp2;
	tmp2 << port;
	for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
	{
		tmp = *it + ':' + tmp2.str();
		if (str == tmp)
			return true;
	}
	return false;
}

bool check_host(std::string client_host, size_t port, std::string address){
	std::string tmp, tmp_local;
    std::ostringstream tmp2;
	tmp2 << port;
	tmp = address + ":" + tmp2.str();
	tmp_local = "localhost:" + tmp2.str();
	if (client_host == tmp || client_host == tmp_local)
		return true;
	return false;
}