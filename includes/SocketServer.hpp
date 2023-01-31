#ifndef SOCKETSERVER_HPP
# define SOCKETSERVER_HPP

# include "Configuration.hpp"
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/epoll.h>
# include <unistd.h>
# include <fcntl.h>
# include <map>
# include <utility>

class SocketServer {

	public:
	SocketServer();
	SocketServer(Configuration conf);
	SocketServer(SocketServer const &src);
	~SocketServer();

	SocketServer	&operator=(SocketServer const &rhs);

	std::vector<Server>			getVctServer() const;
	std::vector<int>			getServerFd() const;
	std::vector<sockaddr_in>	getSockAddr() const;
	int							getEpollFd() const;
	bool						getErrSocket() const;


	private:
	std::vector<Server>			_vctServ;
	std::vector<int>			_serverFd;
	std::vector<sockaddr_in>	_sockAddr;
	int							_epollFd;
	bool						_errSocket;

	void	initSocket();
	void	createSockaddr(int i);
	void	createFdEpoll();
	void	closeSockets();
	int		nonBlockFd(int sockeFd);
	int		isServerFd(int fd) const;
	int		epollWait();
	void	createConnection(int i);


	// int		getIndexConnection(int fd) const;
};

#endif