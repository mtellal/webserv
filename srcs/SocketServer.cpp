#include "../includes/SocketServer.hpp"
#include "../includes/Response.hpp"
#include <string.h>

SocketServer::SocketServer() {}

SocketServer::SocketServer(Configuration conf) : _errSocket(false) {
	this->_vctServ = conf.getVctServer();

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
		this->_vctServ = rhs.getVctServer();
		this->_serverFd = rhs.getServerFd();
		this->_sockAddr = rhs.getSockAddr();
		this->_clientServer = rhs.getClientServer();
		this->_epollFd = rhs.getEpollFd();
		this->_errSocket = rhs.getErrSocket();
	}
	return *this;
}

std::vector<Server>			SocketServer::getVctServer() const {
	return this->_vctServ;
}

std::vector<int>			SocketServer::getServerFd() const {
	return this->_serverFd;
}

std::vector<sockaddr_in>	SocketServer::getSockAddr() const {
	return this->_sockAddr;
}

std::map<int, int>			SocketServer::getClientServer() const {
	return this->_clientServer;
}


int							SocketServer::getEpollFd() const {
	return this->_epollFd;
}

bool						SocketServer::getErrSocket() const {
	return this->_errSocket;
}

void	SocketServer::initSocket() {
	int	socket_fd;
	int	opt = 1;

	for (size_t i = 0; i < this->_vctServ.size(); i++)
	{
		if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		{
			perror("Cannot create socket");
			this->_errSocket = true;
			return ;
		}
		setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		this->_serverFd.push_back(socket_fd);
		this->createSockaddr(i);
		if (bind(socket_fd, (struct sockaddr *)&this->_sockAddr[i], sizeof(this->_sockAddr[i])) == -1)
		{
			perror("Bind failed");
			this->_errSocket = true;
			return ;
		}
		if (nonBlockFd(socket_fd))
			return ;
		// Modifier le 5
		if (listen(socket_fd, 5) == -1)
		{
			perror("listen error");
			this->_errSocket = true;
			return ;
		}
	}
}

void	SocketServer::createSockaddr(int i) {
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	if (this->_vctServ[i].getHost() == "localhost")
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	else
		addr.sin_addr.s_addr = inet_addr(this->_vctServ[i].getHost().c_str());
	addr.sin_port = htons(this->_vctServ[i].getPort());
	// addr.sin_zero ??
	this->_sockAddr.push_back(addr);
}

void	SocketServer::createFdEpoll() {
	struct epoll_event event;

	// Modifier 5
	this->_epollFd = epoll_create(5);
	if (this->_epollFd == -1)
	{
		this->_errSocket = true;
		perror("err epoll_create");
	}
	for (size_t i = 0; i < this->_vctServ.size(); i++)
	{
		event.events = EPOLLIN;
		event.data.fd = this->_serverFd[i];
		if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, this->_serverFd[i], &event) == -1)
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
	for (size_t i = 0; i < this->_vctServ.size(); i++)
	{
		close(this->_serverFd[i]);
		epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, this->_serverFd[i], NULL);
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
	for (size_t i = 0; i < this->_serverFd.size(); i++)
	{
		if (this->_serverFd[i] == fd)
			return i;
	}
	return -1;
}

int		SocketServer::epollWait() {
	// changer 5 (faire une macro pour les events)
	struct epoll_event	event[5];
	int			nbrFd;
	int			i;

	nbrFd = epoll_wait(this->_epollFd, event, 5, -1);
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
		{
			// std::cout << "ServerFd" << std::endl;
			createConnection(i);
		}
		else
		{
			// std::cout << "NotServerFd" << std::endl;
			Request		req(event[j].data.fd);
			if (req.getErrRequest())
				return 1;
			else if (req.getcloseConnection())
				this->closeConnection(event[j].data.fd);
			else
			{
				Response	rep(req, this->getVctServer(), this->getClientServer());
				if (rep.getCloseConnection())
					this->closeConnection(event[j].data.fd);
			}
		}
	}
	return 0;
}

void	SocketServer::createConnection(int i) {
	int					fd;
	struct sockaddr		tmp;
	socklen_t			tmp_len = sizeof(tmp);
	struct epoll_event	event;

	if ((fd = accept(this->_serverFd[i], (struct sockaddr *)&tmp,
			&tmp_len)) == -1)
	{
		perror("err accept");
		this->_errSocket = true;
		return ;
	}
	if (this->nonBlockFd(fd) == 1)
		return ;

	event.events = EPOLLIN;
	event.data.fd = fd;
	this->_clientServer.insert(std::make_pair(fd, i));
	if (epoll_ctl(this->_epollFd, EPOLLIN, fd, &event) == -1)
	{
		perror("err epoll_ctl");
		this->_errSocket = true;
		return ;
	}
}

void	SocketServer::closeConnection(int fd) {
	close(fd);
	epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, fd, NULL);
}
