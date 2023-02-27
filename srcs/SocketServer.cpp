#include "SocketServer.hpp"
#include "Response.hpp"
#include <string.h>
#include <cstring>

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
		this->_clientServerFds = rhs._clientServerFds;
		this->_epollFd = rhs._epollFd;
		this->_errSocket = rhs._errSocket;
		this->_envp = rhs._envp;
	}
	return *this;
}

// GETTER 

std::vector<Server>			SocketServer::getVctServer() const { return this->_servers; }

std::vector<size_t>			SocketServer::getServerFd() const { return this->_servers_fd; }

std::map<int, int>			SocketServer::getClientServer() const { return this->_clientServerFds; }

int							SocketServer::getEpollFd() const { return this->_epollFd; }

bool						SocketServer::getErrSocket() const { return this->_errSocket; }

void	SocketServer::errorSocket(std::string s)
{
	perror(s.c_str());
	_errSocket = true;
	return ;
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
	int				opt = 1;
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

void	SocketServer::createFdEpoll() {
	struct epoll_event event;

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
	for (std::map<int, int>::iterator it = this->_clientServerFds.begin() ; it != this->_clientServerFds.end(); it++)
	{
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
	for (size_t index = 0; index < this->_servers_fd.size(); index++)
	{
		if (this->_servers_fd[index] == (size_t)fd)
			return index;
	}
	return -1;
}

/*
	Compare ip and host from servers in configuration file, 
	with the ip addr of the request (after dns research, ex: localhost => 127.0.0.0.1)
*/

int		SocketServer::pickServBlock(const Request &req)
{
	struct addrinfo	tmp;
	struct addrinfo *res = NULL;
	std::string 	ip;

	memset(&tmp, 0, sizeof(tmp));
	tmp.ai_family = AF_INET;
	tmp.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(req.getHost().c_str(), req.getPort().c_str(), &tmp, &res) == -1)
		perror("getaddrinfo call failed");
	else
		ip = getAddressInfo(*(res->ai_addr));
	freeaddrinfo(res);

	for (size_t i = 0; i < this->_servers.size(); i++)
	{
		if (this->_servers[i].getAddress() == ip
			&& this->_servers[i].getPort() == req.getPort())
		{		
				return (i);
		}
	}
	return (-1);
}

void	printRequest()
{
	std::cout << "\n///////////////////////////////////////////////////////////" << std::endl;
	std::cout <<         "			R E Q U E S T"		 << std::endl;
	std::cout << "///////////////////////////////////////////////////////////" << std::endl;

}

void	printResponse(int end = 0)
{
	std::cout << "\n\n///////////////////////////////////////////////////////////" << std::endl;
	if (end)
		std::cout <<         "		E N D   R E S P O N S E"		 << std::endl;
	else
		std::cout <<         "			R E S P O N S E"		 << std::endl;
	std::cout << "///////////////////////////////////////////////////////////\n" << std::endl;
	if (end)
		std::cout << "\n		//////////////////////////////////////////////////		\n" << std::endl;
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

int		SocketServer::epollWait() {
	struct epoll_event	event[NB_EVENTS];
	int			nbrFd;
	int			index_serv;
	size_t		idx_wreq;
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
		// std::cout << "fd = " << event[j].data.fd << std::endl;
		if (event[j].data.fd == 0)
			return 1;
		if ((index_serv = isServerFd(event[j].data.fd)) >= 0)
			createConnection(index_serv);
		else
		{
			printRequest();

			Request		req(event[j].data.fd);

			if ((idx_wreq = isAwaitingRequest(event[j].data.fd)) != (size_t)-1)
				req = this->_awaitingRequest[idx_wreq];

			req.parsRequest(event[j].data.fd);

			if (req.getEndAwaitingRequest())
					this->_awaitingRequest.erase(this->_awaitingRequest.begin() + idx_wreq);

			if (req.getcloseConnection())
				this->closeConnection(event[j].data.fd);
			else if (req.getAwaitingRequest())
			{
				if (this->isAwaitingRequest(event[j].data.fd) == (size_t)-1)
					this->_awaitingRequest.push_back(req);
				else
					this->_awaitingRequest[idx_wreq] = req;
			}
			else
			{
				if ((srv_i = pickServBlock(req)) == -1)
					std::cerr << "pickServBlock() call failed (verify a serv block exists)" << std::endl;
				else
				{	
					printResponse();
				
					std::cout << "server picked is: " << srv_i << std::endl;

					Response	rep(req, this->_servers[srv_i], this->_envp);
					rep.selectLocationBlock();
					rep.sendData();
					if (rep.getCloseConnection() && !req.getAwaitingRequest())
						this->closeConnection(event[j].data.fd);

					printResponse(1);

				}		
			}
		}
	}
	return 0;
}

void	SocketServer::createConnection(int index_serv)
{
	Client				client;
	int					client_fd;
	struct sockaddr		tmp;
	socklen_t			tmp_len = sizeof(tmp);
	struct epoll_event	event;

	std::cout << "new connection from serv: " << index_serv << std::endl;
	if ((client_fd = accept(this->_servers_fd[index_serv], (struct sockaddr *)&tmp,
			&tmp_len)) == -1)
	{
		perror("accept() call failed");
		this->_errSocket = true;
		return ;
	}
	if (this->nonBlockFd(client_fd) == 1)
		return ;

	client.set(getAddressInfo(tmp), client_fd, tmp);

	this->_servers[index_serv].addClient(client);

	this->_clientServerFds.insert(std::make_pair(client_fd, index_serv));

	event.events = EPOLLIN;
	event.data.fd = client_fd;
	if (epoll_ctl(this->_epollFd, EPOLLIN, client_fd, &event) == -1)
	{
		perror("err epoll_ctl");
		this->_errSocket = true;
		return ;
	}
	
	std::cout << "////////////	NEW CONNECTION 	///////////\n";
	std::cout << client << std::endl;

}

void	SocketServer::closeConnection(int fd)
{
	size_t	index_serv;

	index_serv = _clientServerFds[fd];

	std::cout << "client[" << fd << "] DISCONNECTED\n";
	std::cout << "form: serv[" << index_serv << "] =>" << this->_servers[index_serv].getHost() << ":" << this->_servers[index_serv].getPort() << std::endl; 


	this->_servers[index_serv].eraseClient(fd);

	this->_clientServerFds.erase(fd);

	close(fd);

	epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, fd, NULL);
}
