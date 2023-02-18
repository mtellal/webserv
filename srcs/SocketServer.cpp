#include "SocketServer.hpp"
#include "Response.hpp"
#include <string.h>

SocketServer::SocketServer() {}

SocketServer::SocketServer(Configuration conf, char **envp) : _errSocket(false), _envp(envp) {
	this->_servers = conf.getVctServer();

	this->initSocket();
	if (this->getErrSocket())
		return ;
	this->createFdEpoll();
	while (this->epollWait() != 1)
		;
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
		this->_sockAddr = rhs._sockAddr;
		this->_clientServer = rhs._clientServer;
		this->_epollFd = rhs._epollFd;
		this->_errSocket = rhs._errSocket;
		this->_envp = rhs._envp;
	}
	return *this;
}

// GETTER 

std::vector<Server>			SocketServer::getVctServer() const { return this->_servers; }

std::vector<size_t>			SocketServer::getServerFd() const { return this->_servers_fd; }

std::vector<sockaddr_in>	SocketServer::getSockAddr() const { return this->_sockAddr; }

std::map<int, int>			SocketServer::getClientServer() const { return this->_clientServer; }

int							SocketServer::getEpollFd() const { return this->_epollFd; }

bool						SocketServer::getErrSocket() const { return this->_errSocket; }

void	SocketServer::errorSocket(std::string s)
{
	perror(s.c_str());
	_errSocket = true;
	return ;
}

std::string		getDomainInfo(const struct sockaddr addr)
{
	char	domain[50];

	if (getnameinfo(&addr, sizeof(addr), domain, sizeof(domain), 0, 0, 0) == -1)
		std::cerr << "getnameinfo() call failed" << std::endl;
	//std::cout << "domain: " << domain << std::endl;
	return (domain);
}

std::string		getAddressInfo(const struct sockaddr addr)
{
	char	address[50];

	if (getnameinfo(&addr, sizeof(addr), address, sizeof(address), 0, 0, NI_NUMERICHOST) == -1)
		std::cerr << "getnameinfo() call failed" << std::endl;
	//std::cout << "address: " << address << std::endl;
	return (address);
}

void	SocketServer::initSocket()
{
	int				opt;
	int				serv_socket;
	struct addrinfo hints;
	struct addrinfo *res = NULL;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	for (size_t i = 0; i < _servers.size(); i++)
	{
		if (getaddrinfo(_servers[i].getHost().c_str(), _servers[i].getPort().c_str(), &hints, &res) != 0)
			return (errorSocket("getaddrinfo call failed"));

		// sockaddr == sockaddr_in
		_sockAddr.push_back(*(struct sockaddr_in *)res->ai_addr);
		
		if ((serv_socket = socket((int)res->ai_family, (int)res->ai_socktype, (int)res->ai_protocol)) == -1)
			return (errorSocket("socket call failed"));

		setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

		this->_servers[i].setSocket(serv_socket);
		this->_servers[i].setDomain(getDomainInfo(*res->ai_addr));
		this->_servers[i].setAddress(getAddressInfo(*res->ai_addr));

		//std::cout << "domain: " <<  _servers[i].getDomain() << "\naddress: " << _servers[i].getAddress() << std::endl;

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

void	SocketServer::createFdEpoll() {
	struct epoll_event event;

	this->_epollFd = epoll_create(NB_EVENTS);
	if (this->_epollFd == -1)
	{
		this->_errSocket = true;
		perror("err epoll_create");
	}
	for (size_t i = 0; i < this->_servers.size(); i++)
	{
		event.events = EPOLLIN;
		event.data.fd = this->_servers_fd[i];
		if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, this->_servers_fd[i], &event) == -1)
		{
			this->_errSocket = true;
			perror("err epoll_ctl");
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
	for (std::map<int, int>::iterator it = this->_clientServer.begin() ; it != this->_clientServer.end(); it++)
	{
		this->_clientServer.erase(it->first);
		close(it->first);
		epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, it->first, NULL);
	}
	for (size_t i = 0; i < this->_servers.size(); i++)
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
	for (size_t i = 0; i < this->_servers_fd.size(); i++)
	{
		if (this->_servers_fd[i] == (size_t)fd)
			return i;
	}
	return -1;
}

//	verif si aucun host peut etre envoyer, ex: 'Host: ""' 

int		SocketServer::pickServBlock(const Request &req)
{
	for (size_t i = 0; i < this->_servers.size(); i++)
	{
		if ((this->_servers[i].getAddress() == req.getHost() 
			|| this->_servers[i].getDomain() == req.getHost())
			&& this->_servers[i].getPort() == req.getPort())
		{
			return (i);
		}
	}
	return (-1);
}

int		SocketServer::epollWait() {
	struct epoll_event	event[NB_EVENTS];
	int			nbrFd;
	int			i;
	int 		srv_i;

	nbrFd = epoll_wait(this->_epollFd, event, NB_EVENTS, -1);
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
		if ((i = isServerFd(event[j].data.fd)) >= 0)
			createConnection(i);
		else
		{
			Request		req(event[j].data.fd);

			if ((srv_i = pickServBlock(req)) == -1)
				std::cerr << "pickServBlock() call failed (verify a serv block exists)" << std::endl;

			std::cout << req.getPath() << std::endl;
			if (req.getErrRequest())
				return 1;
			else if (req.getcloseConnection())
				this->closeConnection(event[j].data.fd);
			else
			{
				Response	rep(req, this->_servers[srv_i], this->_envp);
				rep.selectLocationBlock();
				rep.sendData();
				// std::cout << "OK" << std::endl;
				if (rep.getCloseConnection())
					this->closeConnection(event[j].data.fd);
			}
		}
	}
	return 0;
}

void	SocketServer::createConnection(int i)
{
	Client				client;
	int					fd;
	struct sockaddr		tmp;
	socklen_t			tmp_len = sizeof(tmp);
	struct epoll_event	event;

	if ((fd = accept(this->_servers_fd[i], (struct sockaddr *)&tmp,
			&tmp_len)) == -1)
	{
		perror("accept() call failed");
		this->_errSocket = true;
		return ;
	}
	if (this->nonBlockFd(fd) == 1)
		return ;

	client.set(getAddressInfo(tmp), fd, tmp);

	std::cout << "////// New Connection /////\n" << client << std::endl;

	this->_servers[i].addClient(client);

	event.events = EPOLLIN;
	event.data.fd = fd;
	
	//this->_clientServer.insert(std::make_pair(fd, i));
	if (epoll_ctl(this->_epollFd, EPOLLIN, fd, &event) == -1)
	{
		perror("err epoll_ctl");
		this->_errSocket = true;
		return ;
	}
}

void	SocketServer::closeConnection(int fd) {

	//_servers.eraseClient(fd);
	this->_clientServer.erase(fd);
	close(fd);
	epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, fd, NULL);
}
