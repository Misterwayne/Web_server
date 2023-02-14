#include "../header/configParsing.hpp"

ConfigParsing::ConfigParsing() : _clientMaxBodySize(500), _autoindex(false), _uploadEnable(false)
{
	_listen.port = 0;
    init_server_map();
    init_location_map();
}

ConfigParsing::ConfigParsing(size_t &i, std::vector<std::string> const file) : _clientMaxBodySize(500), _autoindex(false), _uploadEnable(false)
{
	_listen.port = 0;
    init_server_map();
    init_location_map();
    this->parsing(i, file);
}

ConfigParsing::ConfigParsing(ConfigParsing const &other) { *this = other; }

ConfigParsing::~ConfigParsing(){}

ConfigParsing &ConfigParsing::operator=(ConfigParsing const &other)
{
	if (this != &other) {
		this->_listen = other._listen;
		this->_root = other._root;
		this->_serverName = other._serverName;
		this->_errorPages = other._errorPages;
		this->_clientMaxBodySize = other._clientMaxBodySize;
		this->_cgiPath = other._cgiPath;
		this->_cgiExtension = other._cgiExtension;
		this->_location = other._location;
		this->_allowedMethods = other._allowedMethods;
		this->_autoindex = other._autoindex;
		this->_uploadPath = other._uploadPath;
		this->_uploadEnable = other._uploadEnable;
		this->_index = other._index;
		this->_alias = other._alias;
	}
	return *this;
}

int ConfigParsing::find_directive(size_t const i, std::vector<std::string> const file, std::string *directive, ServerMap dirMap)
{
    size_t  countArgs = 0;

    if (dirMap.count(file[i]))
    {
        *directive = file[i];
        while (!dirMap.count(file[i + 1 + countArgs]) && file[i + 1 + countArgs] != "location" && file[i + 1 + countArgs] != "}" && (i + 1 + countArgs) != file.size())
            countArgs++;
        return (countArgs);
    }
    return (ERROR);
}

void ConfigParsing::parsing(size_t &i, std::vector<std::string> const file)
{
    int                         countArgs;
    std::vector<std::string>    args;
    std::string                 directive("");

    for (; i < file.size() && file[i] != "}" ; i++)
    {
        if (file[i] != "location")
        {
            countArgs = find_directive(i, file, &directive, _server_map);
            if (countArgs == ERROR)
                error_parsing(file[i]);
            for (; countArgs > 0; countArgs--)
                args.push_back(file[++i]);
            (this->*ConfigParsing::_server_map[directive])(args);
            args.clear();
        }
        else
        {
            ConfigParsing location;
            std::string locArg = file[++i];
            
            if (file[++i] == "{")
            {
                i++;
                for (; i < file.size() && file[i] != "}" ; i++)
                {
                    countArgs = find_directive(i, file, &directive, _location_map);
                    if (countArgs == ERROR)
                        error_parsing(file[i]);
                    for (; countArgs > 0; countArgs--)
                        args.push_back(file[++i]);
                    (location.*ConfigParsing::_location_map[directive])(args);
                    args.clear();
                }
            }
            _location[locArg] = location;
        }
    }
}

void ConfigParsing::skip(size_t &i, std::vector<std::string> const file)
{
    int                         countArgs;
    std::vector<std::string>    args;
    std::string                 directive("");

    for (; i < file.size() && file[i] != "}" ; i++)
    {
        if (file[i] != "location")
        {
            countArgs = find_directive(i, file, &directive, _server_map);
            for (; countArgs > 0; countArgs--)
                i++;
        }
        else
        {
            ConfigParsing location;
            std::string locArg = file[++i];
            
            if (file[++i] == "{")
            {
                i++;
                for (; i < file.size() && file[i] != "}" ; i++)
                {
                    countArgs = find_directive(i, file, &directive, _location_map);
                    for (; countArgs > 0; countArgs--)
                        i++;
                }
            }
        }
    }
}

void    ConfigParsing::add_listen(std::vector<std::string> args)
{
	if (args.size() != 1)
		throw ConfigParsing::InvalidArgsException();
	size_t i = args[0].find_first_of(':'), k = 0;
    if (!isNumber(args[0].substr(i + 1, std::string::npos)))
        throw ConfigParsing::InvalidArgsException();
    _listen.port = std::strtol((args[0].substr(i + 1)).c_str(), NULL, 0);
    if (_listen.port > 65535)
        throw ConfigParsing::InvalidArgsException();
    if (args[0].substr(0, i) != "localhost")
        _listen.host = args[0].substr(0, i);
	else
		_listen.host = "127.0.0.1";
    std::string nb, str = _listen.host;
 	for (size_t i = 0; i < str.size(); i++, k++)
	{
		for (size_t j = 3; std::isdigit(str[i]) && j > 0; j--, i++)
			nb.push_back(str[i]);
		if ((std::atoi(nb.c_str()) > 255 || std::atoi(nb.c_str()) < 0) || (str[i] && str[i] != '.'))
			throw ConfigParsing::InvalidArgsException();
		nb.clear();
	}
	if (k != 4)
		throw ConfigParsing::InvalidArgsException();
}

void    ConfigParsing::add_root(std::vector<std::string> args)
{
    if (args.size() != 1)
        throw ConfigParsing::InvalidArgsException();
    _root = args[0];
}

void    ConfigParsing::add_server_name(std::vector<std::string> args)
{
    if (!args.size())
        throw ConfigParsing::InvalidArgsException();
    for (size_t i = 0; i < args.size(); i++)
        _serverName.push_back(args[i]);
}

void    ConfigParsing::add_error_pages(std::vector<std::string> args)
{
    if (args.size() != 2 || !isNumber(args[0]))
        throw ConfigParsing::InvalidArgsException();
    _errorPages[std::strtol(args[0].c_str(), NULL, 0)] = args[1];
}

void    ConfigParsing::add_client_max_body_size(std::vector<std::string> args)
{
    if (args.size() != 1 || !isNumber(args[0]))
        throw ConfigParsing::InvalidArgsException();
    _clientMaxBodySize = std::strtol(args[0].c_str(), NULL, 0);
}

void    ConfigParsing::add_allowed_methods(std::vector<std::string> args)
{
    if (!args.size())
        throw ConfigParsing::InvalidArgsException();
    for (size_t i = 0; i < args.size(); i++)
    {
        bool p = 0;
        if (args[i] != "GET" && args[i] != "POST" && args[i] != "DELETE")
            throw ConfigParsing::InvalidArgsException();
        for (size_t j = 0; j < _allowedMethods.size(); j++)
            if (!_allowedMethods[j].compare(args[i]))
                p = 1;
        if (!p)
            this->_allowedMethods.push_back(args[i]);
    }
}

void    ConfigParsing::add_index(std::vector<std::string> args)
{
    if (args.size() != 1)
		throw ConfigParsing::InvalidArgsException();
	this->_index = args[0];    
}

void    ConfigParsing::add_auto_index(std::vector<std::string> args)
{
    if (args.size() != 1)
        throw ConfigParsing::InvalidArgsException();
    if (args[0] == "off")
        _autoindex = false;
    else if (args[0] == "on")
        _autoindex = true;
    else
        throw ConfigParsing::InvalidArgsException();

}

void    ConfigParsing::add_upload_enable(std::vector<std::string> args)
{
    if (args.size() != 1)
        throw ConfigParsing::InvalidArgsException();
    if (args[0] == "off")
        _uploadEnable = false;
    else if (args[0] == "on")
        _uploadEnable = true;
    else
        throw ConfigParsing::InvalidArgsException();
}

void    ConfigParsing::add_upload_path(std::vector<std::string> args)
{
    if (args.size() != 1)
        throw ConfigParsing::InvalidArgsException();
    _uploadPath = args[0];
}


void    ConfigParsing::add_cgi_path(std::vector<std::string> args)
{
    if (args.size() != 1)
        throw ConfigParsing::InvalidArgsException();
    _cgiPath = args[0];
}

void    ConfigParsing::add_cgi_extension(std::vector<std::string> args)
{
    if (args.size() != 1)
        throw ConfigParsing::InvalidArgsException();
    _cgiExtension = args[0];
}

void    ConfigParsing::add_alias(std::vector<std::string> args)
{
    if (args.size() != 1)
        throw ConfigParsing::InvalidArgsException();
    _alias = args[0];

}

const char  *ConfigParsing::InvalidArgsException::what() const throw() {  return ("Exception: Invalid Arguments In Configuration File"); }


void ConfigParsing::init_server_map()
{
    _server_map["listen"] = &ConfigParsing::add_listen;
    _server_map["root"] = &ConfigParsing::add_root;
    _server_map["server_name"] = &ConfigParsing::add_server_name;
    _server_map["error_page"] = &ConfigParsing::add_error_pages;
    _server_map["client_max_body_size"] = &ConfigParsing::add_client_max_body_size;
    _server_map["method"] = &ConfigParsing::add_allowed_methods;
    _server_map["index"] = &ConfigParsing::add_index;
    _server_map["autoindex"] = &ConfigParsing::add_auto_index;
    _server_map["cgi_path"] = &ConfigParsing::add_cgi_path;
    _server_map["cgi_extension"] = &ConfigParsing::add_cgi_extension;
    
    _server_map["upload_enable"] = &ConfigParsing::add_upload_enable;
    _server_map["upload_path"] = &ConfigParsing::add_upload_path;
}

void ConfigParsing::init_location_map()
{
    _location_map["root"] = &ConfigParsing::add_root;
    _location_map["error_page"] = &ConfigParsing::add_error_pages;
    _location_map["client_max_body_size"] = &ConfigParsing::add_client_max_body_size;
    _location_map["method"] = &ConfigParsing::add_allowed_methods;
    _location_map["index"] = &ConfigParsing::add_index;
    _location_map["autoindex"] = &ConfigParsing::add_auto_index;
    _location_map["cgi_path"] = &ConfigParsing::add_cgi_path;
    _location_map["cgi_extension"] = &ConfigParsing::add_cgi_extension;
    _location_map["alias"] = &ConfigParsing::add_alias; 
    
	_location_map["upload_enable"] = &ConfigParsing::add_upload_enable;
    _location_map["upload_path"] = &ConfigParsing::add_upload_path;

}

bool    ConfigParsing::checkSession(std::string login) {
    if (_user_session.find(login) != _user_session.end())
        return (true);
    return (false);
}

void    ConfigParsing::setSession(std::string login, std::string password) { _user_session[login] = password; }
void    ConfigParsing::removeSession(std::string login) { _user_session.erase(login); }
bool    ConfigParsing::searchSession(std::string login, std::string password) {
    std::map<std::string, std::string>::const_iterator it = _user_session.find(login);
    if (it != _user_session.end())
        if (it->second == password){
            return true;
        }
    return false;
}

std::ostream	&operator<<(std::ostream &out, ConfigParsing const &server) 
{
    out << "server_name: ";
    for (size_t i = 0; i < server._serverName.size(); i++) {
        out << server._serverName[i];
        if (i != server._serverName.size() - 1)
            out << " ";
    }
    out << std::endl;
    out << "Listen: ";
	out << "[port: " << server._listen.port << " | host: " << server._listen.host << "]" << std::endl;
    out << "root: " << server._root << std::endl;
    out << std::endl;
    out << "error_page: \n";
    for (std::map<int, std::string>::const_iterator i = server._errorPages.begin(); i != server._errorPages.end(); i++) {
        out <<  i->first << " | " << i->second << std::endl;
    }
    out << std::endl;
    out << "client_body_buffer_size: " << server._clientMaxBodySize << std::endl;
    out << "cgi_path:" << server._cgiPath << std::endl;
    out << "cgi_Extension:	" << server._cgiExtension << std::endl;
    out << "allowed methods:";
    for (std::vector<std::string>::const_iterator i = server._allowedMethods.begin(); i != server._allowedMethods.end(); i++)
        out << " " << *i;
    out << std::endl;
    out << "autoindex: " << server._autoindex << std::endl;
    out << "index: ";
	out << server._index << std::endl;
    out << "alias: " << server._alias << std::endl;
    out << "locations: ";
    for (std::map<std::string, ConfigParsing>::const_iterator i = server._location.begin(); i != server._location.end(); i++) {
        out <<  "[" << i->first << "] \n" << i->second << std::endl;
    }
    out << std::endl;
    return out;
}



Listen				        	        ConfigParsing::getListen() const{ return _listen; }
std::string                     	    ConfigParsing::getRoot() const { return _root; }
std::vector<std::string>        	    ConfigParsing::getServerName() const { return _serverName; }
std::map<int, std::string>     		    ConfigParsing::getErrorPages() const { return _errorPages; }
size_t                          	    ConfigParsing::getClientMaxBodySize() const { return _clientMaxBodySize; }
std::vector<std::string>        	    ConfigParsing::getAllowedMethods() const { return _allowedMethods; }
std::string				        	    ConfigParsing::getIndex() const { return _index; }
bool                            	    ConfigParsing::getAutoindex() const { return _autoindex; }
bool                            	    ConfigParsing::getUploadEnable() const { return _uploadEnable; }
std::string                     	    ConfigParsing::getUploadPath() const { return _uploadPath; }
std::string                     	    ConfigParsing::getCgiPath() const { return _cgiPath; }
std::string                     	    ConfigParsing::getCgiExtension() const { return _cgiExtension; }
std::string                     	    ConfigParsing::getAlias() const { return _alias; }
std::map<std::string, ConfigParsing>    ConfigParsing::getLocation() const { return _location; }
