#include "../header/request_handling.hpp"
#include "../header/header.hpp"

Response::Response() : _request(){};

Response::Response(Request *request, ConfigParsing *config) : _method(request->getMethod()), _request(request), _header(request->getHeaders()), _config(config)
{
    _ready = false;
    _delete_on = false;
    _path = ".";
    _code = _request->getStatusCode();
    initStatusCode();
    check_size();
    getContentType();
    if (_code == 200)
    {
        check_path();
        getMethod();
    }
    else
        error_page(_code);
    set_header();
    _response = _return_header + _return_body;
    _size = _response.length();
    _ready = true;
};

Response::~Response(){};

void     Response::error_page(size_t code)
{
    std::ostringstream tmpStatusCode;
    tmpStatusCode << code;
    std::string path;

    path = "www/errorpages/" + tmpStatusCode.str() + ".html";
    _request->setVersion("HTTP/1.1");
    get_page(path);
}

void     Response::check_path()
{
    std::map<std::string, ConfigParsing> tmpConf(_config->getLocation());
	std::string tmpPath = _request->getPath();
	std::map<std::string, ConfigParsing>::iterator it;
    
    while (1)
	{
		it = tmpConf.find(tmpPath);
		if (it != tmpConf.end())
		{
			_location = &(it->second);
			break ;
		}
		if (tmpPath == "/")
		{
			_code = 404;
			break ;
		}
		tmpPath = tmpPath.substr(0, tmpPath.rfind("/"));
		if (tmpPath.empty())
			tmpPath = "/";
	}
    if (!_location->getCgiPath().empty())
        _cgiPath = _location->getCgiPath();
    if (_request->getPath().find(_config->getRoot()) != std::string::npos)
    {
        _path += _request->getPath();
        _path.erase(_path.begin() + 2,  _path.begin() + _path.find_last_of(_config->getRoot()) - 2);
    }
    else if (_request->getPath().find(_location->getRoot()) != std::string::npos)
    {
        _path += _config->getRoot() + _request->getPath();
    }
    else
    {
        _path += _config->getRoot() + _location->getRoot() + _request->getPath();
    }
	if (_request->getPath() == "/")
		_path += _location->getIndex();
    int flag = 0;
    for (unsigned long i = 0; i < _location->getAllowedMethods().size(); i++)
    {
        if ("DELETE" == _location->getAllowedMethods()[i])
            _delete_on = true;
        if (_method == _location->getAllowedMethods()[i])
            flag = 1;
    }
    if (flag == 0)
        _code = 405;

}

void     Response::check_size()
{
    if (_request->getBody().length() > _config->getClientMaxBodySize())
        _code = 413;
    if (_request->getPath().length() > 1024)
        _code = 414;
}

void    Response::set_header()
{
    std::ostringstream tmp;
	tmp << _code;
	_return_header = _request->getVersion() + " " + tmp.str() + " " + _statusCode[_code] + "\r\n";
    if (_isLogged)
        _return_header += "Set-Cookie: session=" + _request->getBody() + "\r\n";
    if (_islogout)
        _return_header += "Set-Cookie: session=\r\n";
    _return_header += "Version: " + _request->getVersion() + "\r\n";
    _return_header += "Connection: keep-alive\r\n";
    _return_header += "Keep-Alive: timeout=10\r\n";
    if (_path.rfind(".")!= std::string::npos && _cgiType.empty())
    {
        size_t i = _path.rfind(".");
        std::string add = _path.substr(i + 1);
        _return_header += "Content-Type: " + _contentType[add] + "\r\n";
    }
    else if (!_cgiType.empty())
        _return_header += "Content-Type: " + _cgiType + "\r\n";
    std::ostringstream tmp2;
    tmp2 << _size;
    _return_header += "Content-Length: " + tmp2.str(); 
    _return_header += "\r\n\r\n";
}

void    Response::getMethod()
{
    if (_code != 200)
    {
        error_page(_code);
        return ;
    }
    if (_method == "GET")
        get_page(_path);
    if (_method == "POST")
    {
        std::string ret = post_page();
        if (ret != "application/x-www-form-urlencoded")
            get_dir(_path);
    }
    if (_method == "DELETE")
    {
        delete_page();
        get_dir(_path.substr(0 ,_path.rfind("/")));
    }
    return ;
}

int     Response::get_page(std::string path)
{
    std::ifstream file;

    if (isDirectory(path) && _location->getAutoindex())
    {
        get_dir(path);
        return 0;
    }
    if (_code == 200 && path.rfind(".") != std::string::npos && path.substr(path.rfind(".") + 1) == _location->getCgiExtension())
    {
        Cgi tmp(_path, _location, _request, _cgiPath);
        _return_header = tmp._header;
        _return_body = tmp._data;
        _cgiType = tmp._cgiType;
        _size = _return_body.length();
        _code = tmp._code;
        return 0;
    }
    if (!isFile(path))
    {
        _code = 404;
        error_page(_code);
        return 0;
    }
    file.open(path.c_str(), std::fstream::in | std::fstream::out);
    if (file.is_open())
    {
        if (_path == "./www/html/loginSession.html" && _header.count("Cookie"))
        {
            if (_header["Cookie"].size() > 8){
                std::string session_path = search_file_session(_header["Cookie"].substr(9));
                if(!session_path.empty())
                {
                    file.close();
                    file.open(session_path.c_str(), std::fstream::in | std::fstream::out);
                }
            }
        }
        else if (_path == "./www/html/logoutSession.html" && _header.find("Cookie") != _header.end())
            _islogout = true;
		std::string buff((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        _return_body = buff + "\r\n";
        _size = buff.length();
    }
    else
    {
        _code = 403;
        error_page(_code);
    }
    file.close();
    return (0);
}

std::string    Response::post_page()
{
	std::string ContentType = _header.at("Content-Type");
    if (ContentType.empty())
    {
        _code = 403;
        return "";
    }
	std::string boundary = "------WebKitFormBoundary";
    if (ContentType.find("--") != std::string::npos)
        boundary = "--" + ContentType.substr(ContentType.find("--"));
    ContentType = ContentType.substr(0, ContentType.find_first_of(";"));
    if (ContentType == "multipart/form-data")
	{
        std::string data;
        std::string file_name;
        std::string fpath;
        if (_request->getBody().empty())
        {
            _code = 403;
            return "";
        }
        data = _request->getBody();
		file_name = data.substr(data.find("filename=\"") + 10);
		file_name = file_name.substr(0, file_name.find_first_of("\""));
		fpath = "./" + _config->getRoot() + "/" + _location->getUploadPath() + "//" + file_name;
		data = data.substr(data.find("\r\n\r\n") + 4);
        data = data.substr(0, data.find(boundary));
        std::fstream file(fpath.c_str(), std::fstream::in | std::fstream::out | std::fstream::trunc);
		if (!file.is_open())
		{
			_code = 403;
			return "";
		}
		file << data;
		file.close();
    }
    else if (ContentType == "application/x-www-form-urlencoded")
    {
        std::string Referer = _header["Referer"];
        std::string filename = Referer.substr(Referer.find_last_of('/') + 1, Referer.size() - Referer.find_last_of('/') - 1);
        std::string session_path = _path;
        if (filename == "setSession.html"){
            if (!create_file_session(_request->getBody()))
                session_path = "./www/html/sessionExist.html";
            get_page(session_path);
        }
        if (filename == "loginSession.html" || filename == "logoutSession.html"){
            session_path = search_file_session(_request->getBody());
            if (session_path.empty()){
                session_path = "./www/html/sessionNotFound.html";
            }
            else
                _isLogged = true;
            get_page(session_path);
        }
    }
	else
		_code = 400;
    return (ContentType);
}

bool    Response::create_file_session(std::string body){
    std::string login = body.substr(body.find_first_of('=') + 1, body.find_first_of('&') - body.find_first_of('=') - 1);
    std::string password = body.substr(body.find_last_of('=') + 1);
    if (!_config->checkSession(login)){
        _config->setSession(login, password);
        std::string filename = "./www/sessions/" + login + ".html";
        std::ofstream newSession(filename.c_str());
        newSession << "<!DOCTYPE html>\n";
        newSession << "<html lang=”en”>\n";
        newSession << "\t<head>\n";
        newSession << "<meta charset=”UTF-8″>\n";
        newSession << "\t\t<title>Session</title>\n";
        newSession << "\t<h1><a href=" << "/" << ">HomePage</a></h1>\n";
        newSession << "\t<h2>Bonjour " << login << "</h2>\n";
        newSession << "\t</head>\n";
        newSession << "\t<body>\n";
        newSession << "\tBienvenue sur la page de session de " << login << ".<br><br>\n";
        newSession << "\t<h4><a href=" << "/logoutSession.html" << ">Logout</a></h1>\n";
        newSession << "\t</body>\n";
        newSession << "<html>\n";
        newSession.close();
    }
    else{
        return false;
    }
    return true;
}

std::string    Response::search_file_session(std::string body) {
    std::string login = body.substr(body.find_first_of('=') + 1, body.find_first_of('&') - body.find_first_of('=') - 1);
    std::string password = body.substr(body.find_last_of('=') + 1);
    std::string filename = "";
    if (_config->searchSession(login, password)) {
        filename = "./www/sessions/" + login + ".html";
        std::ifstream file;
        file.open(filename.c_str());
        if (!file){
            _config->removeSession(login);
            create_file_session(body);
            filename = search_file_session(body);
        }
        else
            file.close();
    }
    return (filename);
}

void    Response::delete_page()
{
    if (isDirectory(_path))
        _code = 403;
    if (access(_path.c_str(), W_OK) != 0)
        _code = 500;
    if (isFile(_path))
    {
        
        if (_location->getUploadPath() == "/sessions"){
            std::string user_remove = _path.substr(_path.find_last_of('/') + 1, _path.size() - _path.find_last_of('/') - 6);
            _config->removeSession(user_remove);
        }       
        if (std::remove(_path.c_str()) != 0)
            _code = 403;
    }
}

void    Response::get_dir(std::string path)
{
        DIR				*dp;
		struct dirent	*di_struct;
		struct stat		file_stats;

		_return_body = "<html>";
        _return_body += "<title>Webserv</title>";
		if (_delete_on)
			_return_body += "<head><script src=\"https://cdnjs.cloudflare.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script><script>function makeDELETErequest(url) {$.ajax({url: url,type: 'DELETE',});}</script></head>";
		_return_body += "<h1>Index of ";
		_return_body += path + "/</h1>";
		_return_body += "<table  style=\"width:100%; text-align:left; line-height: 2\">";
		_return_body += "<tr> <th>Name</th><th>Last modified</th><th>Size</th><th>Action</th> </tr>";
		if ((dp = opendir(_path.c_str())) != NULL)
		{
			while ((di_struct = readdir(dp)) != NULL)
			{
				if (std::string(di_struct->d_name) != "." && std::string(di_struct->d_name) != ".add")
				{
					std::string tmp = _path + "/" + di_struct->d_name;
					stat(tmp.c_str(), &file_stats);
					_return_body += "<tr>";
					_return_body += "<td><a href = \"" + path.substr(path.find(_location->getRoot())) + "/" + std::string(di_struct->d_name) + "\">" + di_struct->d_name;
					if (S_ISDIR(file_stats.st_mode))
						_return_body += "/";
					_return_body += "</a></td>";
					_return_body += "<td>";
					std::tm * ptm = std::localtime(&file_stats.st_mtime);
					char buffer[32];
					std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);
					_return_body += std::string(buffer);
					_return_body += "</td>";
					_return_body += "<td>";
					if (S_ISDIR(file_stats.st_mode))
						_return_body += "directory";
					else
					{
						std::ostringstream tmpSize;
						tmpSize << static_cast<float>(file_stats.st_size) / 1000;
						std::string size = tmpSize.str();
						_return_body +=  size + "Kb";
					}
					_return_body += "</td>";
                    if (_delete_on)
                    {
                        _return_body += "<td><a href = \"" + path.substr(path.find(_location->getRoot())) +"\"><button style=\"cursor: pointer;\" onclick=\"makeDELETErequest('";
						_return_body += _request->getPath() + "/" + std::string(di_struct->d_name) + "')\">DELETE</button><a/> </td>";
                    }
				}
				_return_body += "</tr>";
			} 
		}
		closedir(dp);
		_return_body += "</table></html>\r\n";
		removeDoubleSlash(_return_body);
        _size = _return_body.size();
}

const char    *Response::getResponse()
{
    _size = _response.length();
    return (_response.c_str());
};

void    Response::initStatusCode()
{
        _statusCode[200] = "OK";
        _statusCode[201] = "OK";
        _statusCode[400] = "BAD REQUEST";
        _statusCode[403] = "FORBIDDEN";
        _statusCode[404] = "NOT FOUND";
        _statusCode[405] = "METHOD NOT FOUND";
        _statusCode[413] = "PAYLOAD TOO LARGE";
        _statusCode[414] = "URI TOO LONG";
        _statusCode[415] = "UNSUPORTED MEDIA TYPE";
        _statusCode[418] = "EXPECTATION FAILED";
        _statusCode[500] = "INTERNAL SERVER ERROR";
        _statusCode[501] = "NOT IMPLEMENTED";
        _statusCode[502] = "BAD GATEWAY";
}

void	Response::getContentType() {
	std::vector<std::string> tmp;
    size_t i(0);
    size_t j;
    std::ifstream file("mime.conf");

    if (!file.fail())
    {
        std::string buff((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        while (i != std::string::npos && j != std::string::npos)
        {
            i = buff.find_first_not_of(" \t\n");
            j = buff.find_first_of(" \t\n", i);
            if (j != std::string::npos)
            {
                tmp.push_back(buff.substr(i, j - i));
                buff = buff.substr(j);
            }
            else
                tmp.push_back(buff.substr(i, std::string::npos - 1));
        }
		for (std::vector<std::string>::iterator it = tmp.begin(); it != tmp.end(); it++)
			_contentType[*it] = *it++;
    }
	file.close();
}