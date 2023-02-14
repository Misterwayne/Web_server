#include "../header/server.hpp"

Server::Server(ConfigParsing &conf, Config &allconf) : ready(false), config(&conf), allConfig(&allconf) {
}

Server::Server(Server const &other) {
	*this = other;
}

Server::Server() : ready(false) {}

Server::~Server() {}

Server & Server::operator=(Server const &other) {
	this->socket_server = other.socket_server;
	this->fds = other.fds;
	this->ready = other.ready;
	this->config = other.config;
	this->allConfig = other.allConfig;
	this->client_buff = other.client_buff;
	this->response_buff = other.response_buff;
	return (*this);
}

int		Server::get_socket_server() const {
	return(this->socket_server);
}

bool Server::getReady() {
	return this->ready;
}

void	Server::set_server() {
	struct sockaddr_in addrServer;
	if ((this->socket_server = socket(PF_INET, SOCK_STREAM, 0)) == ERROR)
		throw Server::Errorsocket();
	fcntl(this->socket_server, F_SETFL, O_NONBLOCK);
	int optval = 1;
	if (setsockopt(this->socket_server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(optval)) == ERROR)
		throw Server::Errorsetsockopt();
	memset(&addrServer, 0, sizeof(addrServer));
	addrServer.sin_family = PF_INET;
	addrServer.sin_port = htons(this->config->getListen().port);
	addrServer.sin_addr.s_addr = inet_addr(this->config->getListen().host.c_str());
	if (bind(this->socket_server, (struct sockaddr *)&addrServer, (socklen_t)sizeof(addrServer)) == ERROR)
		throw Server::Errorbind();
	for (size_t i = 0; i < config->getServerName().size(); i++)
		system(create_cmd(config->getServerName()[i], config->getListen().host, config->getListen().port).c_str());
}

void	Server::listen_server() {
	if (listen(this->socket_server, BACKLOG) == ERROR)
		throw Server::Errorlisten();
	struct pollfd fd;
	fd.fd = socket_server;
	fd.events = POLLIN | POLLOUT | POLLRDHUP | POLLERR;
	fds.push_back(fd);
	this->ready = true;
	std::cout << "Server up. " << this->config->getListen().host << ":" << this->config->getListen().port << std::endl;
}

void	Server::add_client() {
	struct pollfd pfd;
	struct sockaddr_in addrClient;
	socklen_t size = sizeof(addrClient);
	memset(&addrClient, 0, sizeof(addrClient));
	if ((pfd.fd = ::accept(this->socket_server, (struct sockaddr *)&addrClient, &size)) == ERROR)
		throw Server::Erroraccept();
	pfd.events = POLLIN | POLLOUT | POLLRDHUP | POLLERR;
	fds.push_back(pfd);
	this->client_buff[pfd.fd].ClientSocket = pfd.fd;
}

int Server::recv(int socket) {
	char buff[BUFF + 1];
	int rd;

	if ((rd = ::recv(socket, buff, BUFF, 0)) == ERROR)
		return (ERROR);
	if (rd == 0)
		return (1);
	buff[rd] = '\0';
	this->client_buff[socket].setRequest(buff, rd);
	client_buff[socket].parsing();
	memset(buff, 0, BUFF);
	return (0);
}

bool Server::send(int socket)
{
	if (response_buff[socket]._ready == false && !client_buff[socket].getRequest().empty())
	{
		client_buff[socket].parsing();
		std::vector<ConfigParsing>::iterator it = search_server(socket);
		Response tmp(&client_buff[socket], &(*it));
		response_buff[socket] = tmp;
		if (response_buff[socket]._ready == false)
		{
			Response tmp(&client_buff[socket], &allConfig->_servers[0]);
			response_buff[socket] = tmp;
		}
	}
	else if (response_buff[socket]._ready == true && !client_buff[socket].getRequest().empty()) 
	{
		int ret;
		if ((ret = ::send(socket, response_buff[socket].getResponse(), response_buff[socket]._size, 0)) == ERROR)
			return false;
		if (!ret)
			return true;
		return true;
	}
	return false;
}

std::vector<ConfigParsing>::iterator	Server::search_server(int socket){
	std::vector<size_t> vec_ret;
	for(size_t i = 0; i < allConfig->_servers.size(); i++){
		if (allConfig->_servers[i].getListen().port == config->getListen().port && allConfig->_servers[i].getListen().host == config->getListen().host)
			vec_ret.push_back(i);
	}
	if (vec_ret.size() > 1 && !check_host(client_buff[socket].getHeaders()["Host"], config->getListen().port, config->getListen().host)){
		for (std::vector<size_t>::iterator it = vec_ret.begin(), ite = vec_ret.end(); it != ite;  it++)
			if (!isHost(client_buff[socket].getHeaders()["Host"], allConfig->_servers[*it].getServerName(), allConfig->_servers[*it].getListen().port))
				vec_ret.erase(it);
	}
	return (allConfig->_servers.begin() + vec_ret[0]);
}

void	Server::routine()
{
	if (poll(&fds[0], fds.size(), TIMEOUT) != ERROR)
	{
		if (this->ready == false)
		{
			this->listen_server();
			return;	
		}	
		else if (fds[0].revents & POLLIN)
		{
			add_client();
			return ;
		}
		for (std::vector<struct pollfd>::iterator it = this->fds.begin(), ite = this->fds.end(); it != ite; ++it)
		{
			if (it->revents & POLLIN)
			{
				int ret = this->recv(it->fd);
				if (ret == 1)
				{
					close(it);
					return;
				}
			}
			else if (it->revents & POLLOUT)
			{
				if (this->send(it->fd) == true)
				{
					close(it);
					return;
				}
			}
			else if (it->revents & POLLRDHUP || it->revents & POLLERR)
			{
				close(it);
				return;
			}
		}
	}
	else 
		throw Server::Errorpoll();
}

void Server::close(std::vector<struct pollfd>::iterator it) {
	this->client_buff.erase(it->fd);
	this->response_buff.erase(it->fd);
	::close(it->fd);
	this->fds.erase(it);	
}
