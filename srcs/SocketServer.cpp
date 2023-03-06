#include "SocketServer.hpp"
#include "Response.hpp"
#include <string.h>
#include <cstring>
#include <signal.h>

SocketServer::SocketServer() {}

SocketServer::SocketServer(Configuration conf, char **envp) : _errSocket(false), _envp(envp) {
	this->_servers = conf.getVctServer();

	this->initSocket();
	if (this->getErrSocket())
		return ;
	this->createFdEpoll();
	std::cout << "\033[1;32mStart Webserv\033[0m" << std::endl << std::endl;
	while (this->epollWait() != 1)
		;
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
		this->_servers = rhs._servers;
		this->_servers_fd = rhs._servers_fd;
		this->_clientServerFds = rhs._clientServerFds;
		this->_epollFd = rhs._epollFd;
		this->_errSocket = rhs._errSocket;
		this->_envp = rhs._envp;
	}
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//												G E T T E R													  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<Server>			SocketServer::getVctServer() const { return this->_servers; }

std::vector<size_t>			SocketServer::getServerFd() const { return this->_servers_fd; }

std::map<int, int>			SocketServer::getClientServer() const { return this->_clientServerFds; }

int							SocketServer::getEpollFd() const { return this->_epollFd; }

bool						SocketServer::getErrSocket() const { return this->_errSocket; }

void	SocketServer::errorSocket(std::string s)
{
	perror(s.c_str());
	this->_errSocket = true;
	return ;
}

std::string		getAddressInfo(const struct sockaddr addr)
{
	char	address[50];

	if (getnameinfo(&addr, sizeof(addr), address, sizeof(address), 0, 0, NI_NUMERICHOST) == -1)
		std::cerr << "getnameinfo() call failed" << std::endl;
	return (address);
}

std::string	SocketServer::getIPFromHostName(const std::string& hostName) {
	struct hostent* host = gethostbyname(hostName.c_str());
	if (!host)
		return "";

	std::stringstream ss;
	ss << inet_ntoa(*(struct in_addr*)host->h_addr);
	return ss.str();
}

bool	SocketServer::hostExist(std::string host) {
	std::vector<std::string> splitHost;

	// if (host == "")
	// 	return true;
	// std::cout << "Host = " << host << std::endl;
	splitHost = ft_split(host.c_str(), ".");
	if ((splitHost.size() == 4 && splitHost[0] == "127") ||
		host == "0.0.0.0")
		return true;

	splitHost = ft_split(getIPFromHostName(host), ".");
	if (splitHost.size() == 4 && splitHost[0] == "127")
		return true;
	return false;
}

bool	SocketServer::hostAlreadySet(size_t maxIdx) {
	std::vector<Server>	vctServ = this->getVctServer();

	for (size_t j = 0; j < maxIdx; j++)
	{
		// if (vctServ[maxIdx].getPort() == vctServ[j].getPort())
		if (vctServ[maxIdx].getPort() == vctServ[j].getPort() &&
			this->hostExist(vctServ[j].getHost()))
			return true;
	}
	return false;
}

void	SocketServer::initSocket()
{
	int				opt = 1;
	int				serv_socket;
	struct addrinfo	hints;
	struct addrinfo	*res = NULL;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	for (size_t i = 0; i < _servers.size(); i++)
	{
		if (!this->hostAlreadySet(i) && this->hostExist(this->_servers[i].getHost()))
		{
			// std::cout << "TEST = " << this->_servers[i].getHost() << std::endl;
			if (getaddrinfo(NULL, _servers[i].getPort().c_str(), &hints, &res) != 0)
				return (errorSocket("getaddrinfo call failed"));
			
			if ((serv_socket = socket((int)res->ai_family, (int)res->ai_socktype, (int)res->ai_protocol)) == -1)
				return (errorSocket("socket call failed"));

			setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

			this->_servers[i].setSocket(serv_socket);
			this->_servers[i].setAddress(getAddressInfo(*res->ai_addr));

			this->_servers_fd.push_back(serv_socket);

			if (bind(serv_socket, res->ai_addr, res->ai_addrlen) == -1)
				return (errorSocket("bind call failed"));
			
			freeaddrinfo((struct addrinfo *)res);
			res = NULL;

			if (nonBlockFd(serv_socket))
				return ;
			if (listen(serv_socket, NB_EVENTS) == -1)
				return (errorSocket("Listen call failed"));
		}
	}
}

void	SocketServer::createFdEpoll() {
	struct epoll_event event;
	int					j = 0;

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
		if (!this->hostAlreadySet(i) && this->hostExist(this->_servers[i].getHost()))
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
	for (size_t index = 0; index < this->_servers_fd.size(); index++)
	{
		if (this->_servers_fd[index] == (size_t)fd)
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

void	SocketServer::printRequest(Request const &req) const {
	std::cout << "\033[1;34mNew request: \033[0m";
	std::cout << "\033[1;37m[" << req.getHost() << ":" << req.getPort() << "]\033[0m";
	std::cout << "\033[1;37m " << req.getMethod() << "\033[0m";
	std::cout << "\033[1;37m " << req.getPath() << "\033[0m" << std::endl;
}

int		SocketServer::epollWait() {
	struct epoll_event	event[NB_EVENTS];
	int			nbrFd;
	int			index_serv;
	int			index_wreq;

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
		// std::cout << "EVENT = " << event[j].data.fd << std::endl;
		if (event[j].data.fd == 0)
			return 1;
		if ((index_serv = isServerFd(event[j].data.fd)) >= 0)
			createConnection(index_serv);
		else
		{			
			Request		req(event[j].data.fd, this->_servers);

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
				this->printRequest(req);
				Response	rep(req, req.getServBlock(), this->_envp);
				rep.selectLocationBlock();
				rep.sendData();
				if (rep.getCloseConnection() && !req.getAwaitingRequest())
					this->closeConnection(event[j].data.fd);	
			}
		}
	}
	return 0;
}



void	SocketServer::createConnection(int index_serv_fd)
{
	Client				client;
	int					client_fd;
	struct sockaddr		tmp;
	socklen_t			tmp_len = sizeof(tmp);
	struct epoll_event	event;

	// std::cout << "////////////	NEW CONNECTION 	///////////\n";

	if ((client_fd = accept(this->_servers_fd[index_serv_fd], (struct sockaddr *)&tmp,
			&tmp_len)) == -1)
	{
		perror("accept() call failed");
		this->_errSocket = true;
		return ;
	}

	if (this->nonBlockFd(client_fd) == 1)
		return ;

	client.set(getAddressInfo(tmp), client_fd, tmp);

	this->_servers[index_serv_fd].addClient(client);

	this->_clientServerFds.insert(std::make_pair(client_fd, index_serv_fd));

	event.events = EPOLLIN;
	event.data.fd = client_fd;
	if (epoll_ctl(this->_epollFd, EPOLLIN, client_fd, &event) == -1)
	{
		perror("err epoll_ctl");
		this->_errSocket = true;
		return ;
	}
	// std::cout << client << std::endl;
}

void	SocketServer::closeConnection(int fd)
{
	// std::cout << "//////////// CLOSE CONNECTION ///////////\n";

	size_t	index_serv_fd;

	index_serv_fd = _clientServerFds[fd];

	// std::cout << "idx Serv Fd = " << index_serv_fd << std::endl;
	// std::cout << "client[" << fd << "] DISCONNECTED\n";
	// std::cout << "form: serv[" << index_serv_fd << "] =>" << this->_servers_fd[index_serv_fd] << ":" << this->_servers_fd[index_serv_fd]<< std::endl; 

	this->_servers[index_serv_fd].eraseClient(fd);

	this->_clientServerFds.erase(fd);

	// std::cout << "fd close = " << fd << std::endl;
	close(fd);
	epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, fd, NULL);
}
