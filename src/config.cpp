#include "../header/config.hpp"

Config::Config(){}

Config::Config(Config const &other) { this->_servers = other._servers; }

Config::~Config(){}

Config &Config::operator=(Config const &other)
{
    if (this != &other)
        this->_servers = other._servers;
    return *this;
}

std::vector<std::string> Config::read_configuration_file(std::string const &file_name)
{
    std::vector<std::string> ret;
    size_t i = 0, j = 0;
    std::ifstream file(file_name.c_str());

    if (!file.fail())
    {
        std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        while (i != std::string::npos && j != std::string::npos)
        {
            i = buffer.find_first_not_of(" \t\n");
            j = buffer.find_first_of(" \t\n", i);
            if (j != std::string::npos)
            {
                ret.push_back(buffer.substr(i, j - i));
                buffer = buffer.substr(j);
            }
            else
                ret.push_back(buffer.substr(i, std::string::npos - 1));
        }
    }
	file.close();
    return (ret);
}

void     Config::parse(std::vector<std::string> const &file)
{
    for (size_t i = 0; i < file.size(); i++)
    {
        if (file[i] == "server" && file[i + 1] == "{")
        {
            i += 2;
            ConfigParsing  server;
            try {
                server.parsing(i, file);
                _servers.push_back(server);
            } catch (std::exception &e) {
                std::cerr << e.what() << std::endl;
                server.skip(i, file);
            }
        }
        else
            error_parsing(file[i]);
    }
}

std::ostream	&operator<<(std::ostream &out, Config const &config)
{
	size_t i(1);
	for (std::vector<ConfigParsing>::const_iterator it = config._servers.begin(); it != config._servers.end(); it++)
	{
		out << "[Server N°" << i++ << "]\n";
		out << *it << "---------------------------------------------\n";
	}
	return out;
} 
