#include "SocketServer.hpp"
#include "Response.hpp"
#include <string.h>
#include <cstring>
#include <signal.h>

SocketServer::SocketServer() {}

SocketServer::SocketServer(Configuration conf, char **envp) :
_errSocket(false), _envp(envp)
{
	this->_servers = conf.getVctServer();

	this->initSocket();
	if (this->getErrSocket())
		return ;
	this->createFdEpoll();
	std::cout << "\033[1;32mStart Webserv\033[0m" << std::endl << std::endl;
	while (this->epollWait() != 1)
	{
		std::cout << "\033[1;97mListening ...\033[0m" << std::flush;
		std::cout << "\r";
	}
	std::cout << "\033[1;31mStop Webserv\033[0m" << std::endl;
	this->closeSockets();
}

SocketServer::SocketServer(SocketServer const &src) {
	*this = src;
}

SocketServer::~SocketServer() {}

SocketServer	&SocketServer::operator=(SocketServer const &rhs) {
	if (this != &rhs)
	{
		this->_errSocket = rhs._errSocket;
		this->_epollFd = rhs._epollFd;
		this->_envp = rhs._envp;
		this->_servers_fd = rhs._servers_fd;
		this->_servers = rhs._servers;
		this->_awaitingRequest = rhs._awaitingRequest;
		this->_clientServerFds = rhs._clientServerFds;
	}
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//												G E T T E R													  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool						SocketServer::getErrSocket() const { return this->_errSocket; }

std::vector<Server>			SocketServer::getVctServer() const { return this->_servers; }

std::string		getAddressInfo(const struct sockaddr addr)
{
	char	address[50];

	if (getnameinfo(&addr, sizeof(addr), address, sizeof(address), 0, 0, NI_NUMERICHOST) == -1)
		std::cerr << "getnameinfo() call failed" << std::endl;
	return (address);
}

void	SocketServer::errorSocket(std::string s, struct addrinfo *res)
{
	if (res)
		freeaddrinfo((struct addrinfo *)res);
	for (size_t j = 0; j < this->_servers_fd.size(); j++)
		close(this->_servers_fd[j]);
	perror(s.c_str());
	this->_errSocket = true;
	return ;
}

void	SocketServer::closeFdSocket(int i, int err) {
	for (int j = 0; j < i; j++)
		close(this->_servers[j].getFd());
	this->_errSocket = true;
	std::cerr << gai_strerror(err) << std::endl;
}

void	SocketServer::initSocket()
{
	int				opt = 1;
	int				serv_socket;
	int				ret;
	struct addrinfo	hints;
	struct addrinfo	*res = NULL;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	for (size_t i = 0; i < _servers.size(); i++)
	{
		if ((ret = getaddrinfo(_servers[i].getHost().c_str(), _servers[i].getPort().c_str(), &hints, &res)) != 0)
			return (this->closeFdSocket(i, ret));
		

		if ((serv_socket = socket((int)res->ai_family, (int)res->ai_socktype, (int)res->ai_protocol)) == -1)
			return (errorSocket("socket call failed", res));

		setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

		this->_servers[i].setSocket(serv_socket);
		this->_servers[i].setAddress(getAddressInfo(*res->ai_addr));

		this->_servers_fd.push_back(serv_socket);

		if (bind(serv_socket, res->ai_addr, res->ai_addrlen) == -1)
			return (errorSocket("bind call failed", res));
		
		freeaddrinfo((struct addrinfo *)res);
		res = NULL;

		if (nonBlockFd(serv_socket))
			return ;
		if (listen(serv_socket, NB_EVENTS) == -1)
			return (errorSocket("Listen call failed", res));
	}
}

void	SocketServer::createFdEpoll() {
	struct epoll_event event;
	int					j = 0;

	memset(&event, 0, sizeof(event));
	this->_epollFd = epoll_create(NB_EVENTS);
	if (this->_epollFd == -1)
	{
		this->_errSocket = true;
		perror("err epoll_create");
	}
	if (this->nonBlockFd(this->_epollFd) == 1)
		return ;
	for (size_t i = 0; i < this->_servers.size(); i++)
	{
			event.events = EPOLLIN;
			event.data.fd = this->_servers_fd[j];
			if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, this->_servers_fd[j], &event) == -1)
			{
				this->_errSocket = true;
				perror("err epoll_ctl");
			}
			j++;
	}
	event.events = EPOLLIN;
	event.data.fd = 0;
	if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, 0, &event) == -1)
	{
		this->_errSocket = true;
		perror("err epoll_ctl");
	}
}

void	SocketServer::closeSockets() {
	for (std::map<int, int>::iterator it = this->_clientServerFds.begin() ; it != this->_clientServerFds.end(); it++)
	{
		close(it->first);
		epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, it->first, NULL);
	}
	for (size_t i = 0; i < this->_servers_fd.size(); i++)
	{
		close(this->_servers_fd[i]);
		epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, this->_servers_fd[i], NULL);
	}
	epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, 0, NULL);
	close(this->_epollFd);
}

// fcntl seulement pour macos ou ok pour linux ?
int		SocketServer::nonBlockFd(int socketFd) {
	int arg = fcntl(socketFd, F_GETFL);

	if (arg == -1)
	{
		perror("fcntl F_GETFL");
		this->_errSocket = true;
		return 1;
	}
	if ((fcntl(socketFd, F_SETFL, arg | O_NONBLOCK)) == -1)
	{
		perror("fcntl O_NONBLOCK");
		this->_errSocket = true;
		return 1;
	}
	return 0;
}

int		SocketServer::isServerFd(int fd) const {
	for (size_t index = 0; index < this->_servers.size(); index++)
	{
		if (this->_servers[index].getFd() == (size_t)fd)
			return index;
	}
	return -1;
}

size_t	SocketServer::isAwaitingRequest(int fd)
{
	for (size_t i = 0; i < this->_awaitingRequest.size(); i++)
	{
		if (fd == this->_awaitingRequest[i].getFd())
			return (i);
	}
	return (-1);
}

void	handler(int signal) {
	(void)signal;
}

int		SocketServer::indexServFromFd(int fd)
{
	for (size_t i = 0; i < this->_servers.size(); i++)
	{
		if ((int)this->_servers[i].getFd() == fd)
			return (i);
	}
	return (-1);
}

int		SocketServer::epollWait() {
	struct epoll_event	event[NB_EVENTS];
	int					nbrFd;
	int					index_serv;
	int					index_wreq;

	signal(SIGINT, &handler);
	nbrFd = epoll_wait(this->_epollFd, event, NB_EVENTS, 1000);
	if (nbrFd == -1)
	{
		perror("epoll_wait");
		this->_errSocket = true;
		return 1;
	}
	for (int j = 0; j < nbrFd; j++)
	{
		if (event[j].data.fd == 0)
			return 1;
		else if ((index_serv = isServerFd(event[j].data.fd)) >= 0)
			createConnection(index_serv);
		else
		{
			Request	req(event[j].data.fd, this->_servers, this->_clientServerFds, this->_epollFd);

			if ((index_wreq = isAwaitingRequest(event[j].data.fd)) != -1)
				req = this->_awaitingRequest[index_wreq];

			req.request(event[j].data.fd);

			if (!req.getAwaitingRequest() && index_wreq != -1)
					this->_awaitingRequest.erase(this->_awaitingRequest.begin() + index_wreq);

			if (req.getcloseConnection())
				this->closeConnection(event[j].data.fd);
			else if (req.getAwaitingRequest())
			{
				if (this->isAwaitingRequest(event[j].data.fd) == (size_t)-1)
					this->_awaitingRequest.push_back(req);
				else
					this->_awaitingRequest[index_wreq] = req;
			}
			else
			{
				Response	rep(req, req.getServBlock(), this->_envp);

				rep.sendData();
				if (rep.getCloseConnection() && !req.getAwaitingRequest())
					this->closeConnection(event[j].data.fd);
			}
		}
	}

	return 0;
}

void	SocketServer::displayNewConnection(const Server &serv, const std::string &clientIP) const 
{
	time_t		t;
	std::string	_time;

	std::time(&t);
	_time = std::ctime(&t);
	std::cout << "\033[1;34m[" << _time.substr(0, _time.length() - 1) << "]\033[0m";
	std::cout << "\033[1;36m [NEW CONNECTION] \033[0m";
	std::cout << "\033[1;97m[" << serv.getHost() << ":" << serv.getPort() << "]\033[0m";
	std::cout << "\033[1;97m - " << clientIP << "\033[0m" << std::endl;
}

void	SocketServer::createConnection(int index_serv_fd)
{
	Client				client;
	int					client_fd;
	struct sockaddr		tmp;
	socklen_t			tmp_len = sizeof(tmp);
	struct epoll_event	event;

	if ((client_fd = accept(this->_servers[index_serv_fd].getFd(), (struct sockaddr *)&tmp,
			&tmp_len)) == -1)
	{
		perror("accept() call failed");
		this->_errSocket = true;
		return ;
	}

	// std::cout << "ACCEPT Server: " << index_serv_fd << std::endl;

	if (this->nonBlockFd(client_fd) == 1)
		return ;

	client.set(getAddressInfo(tmp), client_fd, tmp);

	this->_servers[index_serv_fd].addClient(client);

	displayNewConnection(this->_servers[index_serv_fd], client.getIpAddress());

	this->_clientServerFds.insert(std::make_pair(client_fd, index_serv_fd));

	memset(&event, 0, sizeof(event));
	event.events = EPOLLIN;
	event.data.fd = client_fd;
	if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, client_fd, &event) == -1)
	{
		perror("err epoll_ctl");
		this->_errSocket = true;
		return ;
	}
}

void	SocketServer::displayDisconnection(const Server &serv, const std::string &clientIP) const
{
	time_t		t;
	std::string	_time;

	std::time(&t);
	_time = std::ctime(&t);
	std::cout << "\033[1;34m[" << _time.substr(0, _time.length() - 1) << "]\033[0m";
	std::cout << "\033[1;36m [DISCONNECTION] \033[0m";
	std::cout << "\033[1;97m[" << serv.getHost() << ":" << serv.getPort() << "]\033[0m";
	std::cout << "\033[1;97m - " << clientIP << "\033[0m" << std::endl;
}

void	SocketServer::closeConnection(int fd)
{
	size_t	index_serv_fd;

	index_serv_fd = _clientServerFds[fd];

	displayDisconnection(this->_servers[index_serv_fd],
	this->_servers[index_serv_fd].getClient(fd).getIpAddress());

	this->_servers[index_serv_fd].eraseClient(fd);

	this->_clientServerFds.erase(fd);

	close(fd);
	epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, fd, NULL);
}
