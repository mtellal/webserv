#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <map>
# include <vector>
# include <string>
# include <fstream>
# include <cstring>
# include <iostream>
# include <arpa/inet.h>
# include <sys/epoll.h>
# include <sys/socket.h>
# include <ctime>

# include "Server.hpp"


# define BUFFLEN 4096
# define BUFFLEN_FILE 65536

class Request {

	public:
	
		Request();
		Request(int fd, const std::vector<Server> &servers);
		Request(Request const &src);
		~Request();

		Request	&operator=(Request const &rhs);

		void								request(int fd);
		void								setErrorRequest(bool);
		void								setBytesRecieved(size_t bytes);
		bool								getAgentSet() const;
		bool								getAcceptSet() const;
		bool								getErrRequest() const;
		bool								getRefererSet() const;
		bool								getBadRequest() const;
		bool								getConnectionSet() const;
		bool								getBodyFileExists() const;
		bool								getcloseConnection() const;
		bool								getAwaitingRequest() const;
		int									getFd() const;
		size_t								getBytesRecievd() const;
		std::string							getPath() const;
		std::string							getHost() const;
		std::string							getPort() const;
		std::string							getAgent() const;
		std::string							getAccept() const;
		std::string							getMethod() const;
		std::string							getReferer() const;
		std::string							getHttpVersion() const;
		std::string							getConnection() const;
		std::string							getServerName() const;
		std::string							getContentType() const;
		std::string							getQueryString() const;
		std::string							getCgiExtension() const;
		std::string							getContentLength() const;
		std::string							getAuthentification() const;
		Server								getServBlock() const;


	private:

		bool								_hostSet;
		bool								_tooLarge;
		bool								_agentSet;
		bool								_acceptSet;
		bool								_methodSet;
		bool								_errRequest;
		bool								_refererSet;
		bool								_badRequest;
		bool								_boundarySet;
		bool								_awaitingBody;
		bool								_connectionSet;
		bool								_queryStringSet;
		bool								_bodyFileExists;
		bool								_awaitingHeader;
		bool								_closeConnection;

		int									_fd;
		size_t								_bodyBytesRecieved;
		
		std::string							_host;
		std::string							_path;
		std::string							_port;
		std::string							_agent;
		std::string							_errMsg;
		std::string							_method;
		std::string							_accept;
		std::string							_referer;
		std::string							_request;
		std::string							_boundary;
		std::string							_connection;
		std::string							_serverName;
		std::string							_httpVersion;
		std::string							_queryString;
		std::string							_contentType;
		std::string							_bodyFilePath;
		std::string							_cgiExtension;
		std::string							_contentLength;
		std::string							_authentification;

		std::vector<Server>					_servers;
		Server								_servBlock;


		void			(Request::*functPtr[12])(std::vector<std::string>);
		void			verifyFiles();
		void			quitRequest();
		void			checkCgiPath();
		void			printRequest() const;
		void			awaitingBody(int fd);
		void			quitAwaitingRequest();
		void			checkBodyBytesRecieved();
		void			parsArgs(std::string tmp);
		void			bodyRequest(size_t index);
		void			setHTTPFields(const std::string &header);
		void			extractFile(const std::string &bound_data);
		void			setAgent(std::vector<std::string> strSplit);
		void			setAccept(std::vector<std::string> strSplit);
		void			setReferer(std::vector<std::string> strSplit);
		bool			setMethodVersionPath(const std::string &line);
		void			setHostPort(std::vector<std::string> strSplit);
		void			setConnection(std::vector<std::string> strSplit);
		void			setContentType(std::vector<std::string> strSplit);
		void			setContentLength(std::vector<std::string> strSplit);
		void			bodyRequest(const std::string &body, size_t &total);
		void			setGetParams(std::vector<std::string> vct, size_t *i);
		void			setAuthentification(std::vector<std::string> strSplit);
		void			getErrorPage(const std::string &errMsg, int status = 500);
		void			addQueryString(const std::string &key, const std::string &value);
		void			extractContentType(const std::string &line, std::string &contentType);
		void			extractContentDisposition(const std::string &line, std::string &name, std::string &filename);

		int				setServBlock();
		int				pickServBlock();
		int				awaitingHeader(int fd);
		int				openBodyFile(std::ofstream &out);
		int				recvToBodyFile(int fd, std::ofstream &out);
		int				selectBlockWithServerName(std::vector<Server> vctServSelect, std::vector<int> index);

		std::string		getRightHost(const std::string& host);
		std::string		getHostNameFromIP(const std::string& ipAddress);

};

std::ostream &operator<<( std::ostream & o, Request const & rhs);

#endif