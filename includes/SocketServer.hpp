#ifndef SOCKETSERVER_HPP
# define SOCKETSERVER_HPP

# include "Configuration.hpp"

# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/types.h>
#include <netdb.h>
#include <cstdlib>
#include <stdio.h>

# include <arpa/inet.h>
# include <sys/epoll.h>

# include <stdlib.h>

# include <unistd.h>
# include <fcntl.h>
# include <map>
# include <utility>

# include "Client.hpp"
#include "Request.hpp"

# define NB_EVENTS 255

class SocketServer {

	public:

		SocketServer();
		SocketServer(Configuration conf, char **envp);
		SocketServer(SocketServer const &src);
		~SocketServer();

		SocketServer	&operator=(SocketServer const &rhs);

		std::vector<Server>			getVctServer() const;
		std::vector<size_t>			getServerFd() const;
		std::map<int, int>			getClientServer() const;
		int							getEpollFd() const;
		bool						getErrSocket() const;


	private:

		std::vector<Server>			_servers;
		std::vector<size_t>			_servers_fd;
		std::map<int, int>			_clientServerFds;
		int							_epollFd;
		bool						_errSocket;
		char						**_envp;
		std::vector<Request>		_awaitingRequest;

		void						errorSocket(std::string s);
		void						initSocket();
		void						createSockaddr(int i);
		void						createFdEpoll();
		void						closeSockets();
		int							nonBlockFd(int sockeFd);
		int							isServerFd(int fd) const;
		int							epollWait();
		void						createConnection(int i);
		void						closeConnection(int fd);

		int							pickServBlock(const Request &req);

		size_t						isAwaitingRequest(int fd);


};

#endif