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

		bool						getErrSocket() const;
		int							getEpollFd() const;
		std::vector<size_t>			getServerFd() const;
		std::vector<Server>			getVctServer() const;
		std::map<int, int>			getClientServer() const;


	private:

		bool						_errSocket;
		int							_epollFd;
		char						**_envp;
		std::vector<size_t>			_servers_fd;
		std::vector<Server>			_servers;
		std::vector<Request>		_awaitingRequest;
		std::map<int, int>			_clientServerFds;

		void						initSocket();
		void						closeSockets();
		void						createFdEpoll();
		void						closeConnection(int fd);
		void						errorSocket(std::string s);
		void						createConnection(int index_serv_fd);
		void						displayNewConnection(const Server &serv, const std::string &clientIP) const; 
		void						displayDisconnection(const Server &serv, const std::string &clientIP) const; 

		
		bool		 				hostExist(std::string host);
		bool						hostAlreadySet(size_t maxIdx);
		
		int							epollWait();
		int							indexServFromFd(int fd);
		int							nonBlockFd(int sockeFd);
		int							isServerFd(int fd) const;
		int							pickServBlock(const Request &req);
		int							selectBlockWithServerName(std::vector<Server> vctServSelect, std::vector<int> index, const Request &req);
		
		size_t						isAwaitingRequest(int fd);
		
		void						printRequest(Request const &req) const;
		
		std::string 				getRightHost(const std::string& host);
		std::string					getHostNameFromIP(const std::string& ipAddress);

};

#endif