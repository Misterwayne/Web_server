#ifndef _CONFIG_HPP
# define _CONFIG_HPP

# include "header.hpp"

class ConfigParsing;

class Config
{
    public:
        Config();
        Config(Config const &other);
        ~Config();

        Config  &operator=(Config const &other);

        
        std::vector<std::string>    read_configuration_file(std::string const &file_name);
        void                        parse(std::vector<std::string> const &file);

        friend  std::ostream &operator<<(std::ostream &out, Config const &config);

        std::vector<ConfigParsing>    _servers;
};

#endif