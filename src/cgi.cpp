#include "../header/cgi.hpp"

Cgi::Cgi() {};

Cgi::Cgi(std::string path, const ConfigParsing *conf, Request *req, std::string cgiPath) : _config(conf), _path(path)
{
    _request = req;
    int input = dup(STDIN_FILENO);
    int output = dup(STDOUT_FILENO);
    FILE *filein = tmpfile();
    FILE *fileout = tmpfile();
    int fdin = fileno(filein);
    int fdout = fileno(fileout);
    lseek(fdin, 0, SEEK_SET);

	pid_t pid = fork();
	if (pid == -1)
		_code = 502;
	if (pid == 0)
	{
		extern char **environ;
		char const	*progInfo[3];
		progInfo[0] = cgiPath.c_str();
		progInfo[1] = _path.c_str();
		progInfo[2] = NULL;
		
		dup2(fdin, STDIN_FILENO);
		dup2(fdout, STDOUT_FILENO);
		if (execve(progInfo[0], (char *const *)progInfo, environ) == -1)
			exit(1);
	}
	int status;
	if (waitpid(pid, &status, 0) == -1)
		_code  = 502;
	if (WIFEXITED(status) && WEXITSTATUS(status))
		_code  = 502;
	lseek(fdout, 0, SEEK_SET);
	char buffer[BUFF];
	int rd;
	std::string tmp;
	while ((rd = read(fdout, buffer, BUFF - 1)) != 0) 
	{
		if (rd == -1) 
		{
			_code  = 502;
			break;
		}
		tmp.append(buffer, rd);
	}
	dup2(STDIN_FILENO, input);
	dup2(STDOUT_FILENO, output);
	close(input);
	close(output);
	close(fdin);
	close(fdout);
	fclose(filein);
	fclose(fileout);
	if (_request->getStatusCode() == 200)
	{
		size_t i = tmp.find("\r\n\r\n");
		if (i != std::string::npos)
		{
			std::string tmp2 = tmp.substr(0, i);
			std::string tmp3 = tmp2.substr(tmp2.find("Content-type:") + 13, std::string::npos);
			_cgiType = tmp3.substr(tmp3.find_first_not_of(" "), tmp3.find_first_of(";") - 1);
			_data = tmp.substr(tmp.find("\r\n\r\n") + 4, std::string::npos);
		}
		else
			_data = tmp;
	}
};

Cgi::Cgi(const Cgi &other)
{
    _address = other._address;
    _cgiPath = other._cgiPath;
    _config =  other._config;
    _data = other._data;
    _filename = other._filename;
    _header = other._header;
    _path = other._path;
    _request = other._request;
};

Cgi::~Cgi() {};