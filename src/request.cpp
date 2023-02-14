#include "../header/request.hpp"

void	Request::parsing() {
	std::string line, key, value;
	size_t j = 0, i = 0;

	initHeaders();
	initMethodList();
	try {
		parseFirstLine(gnl(_request, i));
	}
	catch (const std::exception &e){
		e.what();
		if (_statusCode != 405)
			_statusCode = 400;
	}
	while(((line = gnl(_request, i)) != "") && _statusCode != 400)
    {
        j = line.find_first_of(':');
        if (j != std::string::npos)
		{
			key = line.substr(0, j);
			if (!key.empty())
			{
				key.erase(key.find_last_not_of(" \t") + 1);
				key.erase(0, key.find_first_not_of(" \t"));
			}
			value = line.substr(j + 1);
			if (!value.empty())
			{
				value.erase(value.find_last_not_of(" \t") + 1);
				value.erase(0, value.find_first_not_of(" \t"));
			}
			_headers[key] = value;
		}
		else
			_statusCode = 400;
    }
	if (_headers.empty())
		_statusCode = 400;
	if (_statusCode == 200 && i != std::string::npos)
		_body = _request.substr(i);
}

std::string Request::gnl(std::string const &str, size_t &i)
{
    std::string ret;
    size_t      j;

    if (i == std::string::npos)
        return "";
    ret = str.substr(i);
	j = ret.find_first_of('\n');
    ret = ret.substr(0, j);
    if (ret[ret.size() - 1] == '\r')
		ret.resize(ret.size() - 1);
	i = ((j == std::string::npos) ? j : j + i + 1);
    return ret;
}

void    Request::parseFirstLine(std::string const &str)
{
    size_t i = 0, j = -1;
	std::string line;

	i = str.find_first_of(' ');
	if (i == std::string::npos)
		throw Request::ErrorMethodSpace();
	_method.assign(str, 0, i);
	while (++j < _methodList.size())
		if (_method == _methodList[j])
			break ;
	if (j == _methodList.size()){
		_statusCode = 405;
		throw Request::ErrorMethod();
	}
	line = str.substr(i + 1);
	i = line.find_first_of(' ');
	if (i == std::string::npos)
		throw Request::ErrorPathAndVersion();
	_path.assign(line, 0, i);
	for (size_t k = 0; k < _path.size(); k++)
		if (_path[k] == '%' && _path[k + 1] && _path[k + 2] && _path[k + 1] == '2' && _path[k + 2] == '0')
			_path.replace(k, 3, " ");
	j = _path.find_first_of('?');
	if (j != std::string::npos){
		_query = _path.substr(j + 1);
		_path = _path.substr(0, j);
	}
	line = line.substr(i + 1);
	if (line.empty())
		throw Request::ErrorVersion();
	if ((line.compare(0, 8, "HTTP/1.1") == 0 || line.compare(0, 8 , "HTTP/1.0") == 0) && line.size() == 8)
		_version.assign(line, 0, 8);
	else
		throw Request::ErrorHttpVersion();
}

void Request::initMethodList() {
    _methodList.push_back("GET");
	_methodList.push_back("DELETE");
	_methodList.push_back("POST");


}

void Request::initHeaders() {
    this->_headers.clear();
}

Request::Request() : _body(""), _statusCode(200) {}

Request::Request(std::string const &str) : 
    _body(""), _statusCode(200), _rawString(str)
{
    parsing();
    if (this->_statusCode != 200)
        std::cerr << "Parsing Error : " << this->_statusCode << std::endl;
}

Request::~Request(){}

Request::Request(Request const &other) {
	*this = other;
}

Request &Request::operator=(Request const &rhs) {
	if (this == &rhs)
		return (*this);
    this->_method = rhs._method;
    this->_path = rhs._path;
    this->_query = rhs._query;
    this->_version = rhs._version;
    this->_methodList = rhs._methodList;
    this->_headers = rhs._headers;
    this->_body = rhs._body;
    this->_statusCode = rhs._statusCode;
    this->_rawString = rhs._rawString;
	this->_request = rhs._request;
	this->ClientSocket = rhs.ClientSocket;
    return (*this);
}

void			Request::clearAll()
{
	_method.clear();
	_path.clear();
	_query.clear();
	_version.clear();
	_methodList.clear();
	_headers.clear();
	_body.clear();
	_statusCode = 200;
	_rawString.clear();
	_request.clear();
	ClientSocket = 0;
}

void								Request::setStatusCode(size_t statusCode) { this->_statusCode = statusCode; }
void								Request::setVersion(std::string const &str) { this->_version = str; }
void								Request::setRequest(char *buff, int size) { _request.append(buff, size); }


std::map<std::string, std::string>  Request::getHeaders() const { return this->_headers; }
std::string                         Request::getBody() const { return _body; }
size_t                              Request::getStatusCode() const { return this->_statusCode; }
std::string                         Request::getRawString() const {return this->_rawString; }
std::string							Request::getMethod() const { return this->_method; }
std::string							Request::getQuery() const { return this->_query; }
std::string							Request::getPath() const {return this->_path; }
std::string							Request::getVersion() const { return this->_version; }
std::string							Request::getRequest() const { return this->_request; }

