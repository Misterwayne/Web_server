#include "../header/header.hpp" 
#include "../header/server.hpp" 

int main(int ac, char **av) 
{
	Config 						config;
	std::vector<std::string>	file;
	std::vector<Server>			servers;

	if (ac != 2)
  	{
		std::cerr << "Not Enough Arguments" << std::endl;
    	return (EXIT_FAILURE);
  	}
	file = config.read_configuration_file(av[1]);
	if (file.empty())
	{
		std::cerr << "Configuration File Empty Or Not Found" << std::endl;
		return (EXIT_FAILURE);
	}
	try {
		config.parse(file);
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}
	if (config._servers.empty())
		return (EXIT_FAILURE);
	for (std::vector<ConfigParsing>::iterator it = config._servers.begin(); it != config._servers.end(); it++) {
		Server	serv(*it, config);
		try {
			serv.set_server();
		}
		catch(const std::exception& e) {
			std::cerr << e.what() << strerror(errno) << std::endl;
			return (EXIT_FAILURE);
		}
		servers.push_back(serv);
	}
	while (1) 
	{	
		for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++) 
		{
			try {
				it->routine();
			}
			catch(const std::exception& e) {
				std::cerr << e.what() << strerror(errno) << std::endl;
				return (EXIT_FAILURE);
			}
		}
	}
	return (EXIT_SUCCESS);
}
