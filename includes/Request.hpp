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

		bool								getErrRequest() const;
		bool								getcloseConnection() const;
		bool								getAwaitingRequest() const;
		bool								getConnectionSet() const;
		bool								getAcceptSet() const;
		bool								getRefererSet() const;
		bool								getAgentSet() const;
		bool								getBadRequest() const;
		bool								getBodyFileExists() const;
		int									getFd() const;
		size_t								getBytesRecievd() const;
		std::string							getMethod() const;
		std::string							getPath() const;
		std::string							getHttpVersion() const;
		std::string							getHost() const;
		std::string							getPort() const;
		std::string							getConnection() const;
		std::string							getAccept() const;
		std::string							getReferer() const;
		std::string							getAgent() const;
		std::string							getServerName() const;
		std::string							getAuthentification() const;
		std::string							getContentLength() const;
		std::string							getContentType() const;
		std::string							getQueryString() const;
		void								setBytesRecieved(size_t bytes);
		Server								getServBlock() const;
		std::string							getCgiExtension() const;


	private:

		int									_fd;
		std::vector<Server>					_servers;
		bool								_errRequest;
		bool								_queryStringSet;
		bool								_boundarySet;
		bool								_closeConnection;
		std::string							_boundary;
		std::string							_method;
		std::string							_path;
		std::string							_httpVersion;
		std::string							_host;
		std::string							_port;
		std::string							_connection;
		bool								_connectionSet;
		std::string							_accept;
		bool								_acceptSet;
		std::string							_queryString;
		bool								_refererSet;
		std::string							_referer;
		bool								_agentSet;
		std::string							_agent;
		std::string							_serverName;
		std::string							_authentification;
		std::string							_contentLength;
		std::string							_contentType;
		bool								_methodSet;
		bool								_hostSet;
		bool								_tooLarge;
		bool								_badRequest;

		size_t								_bodyBytesRecieved;
		bool								_bodyFileExists;
		std::string							_bodyFilePath;

		std::string							_request;

		bool								_awaitingHeader;
		bool								_awaitingBody;

		Server								_servBlock;

		std::string							_cgiExtension;

		void			(Request::*functPtr[12])(std::vector<std::string>);
		void			parsArgs(std::string tmp);
		bool			setMethodVersionPath(const std::string &line);
		void			setHostPort(std::vector<std::string> strSplit);
		void			setConnection(std::vector<std::string> strSplit);
		void			setAccept(std::vector<std::string> strSplit);
		void			setReferer(std::vector<std::string> strSplit);
		void			setAgent(std::vector<std::string> strSplit);
		void			setAuthentification(std::vector<std::string> strSplit);
		void			setContentLength(std::vector<std::string> strSplit);
		void			setContentType(std::vector<std::string> strSplit);
		void			setGetParams(std::vector<std::string> vct, size_t *i);
		void			verifyFiles();
		void			setHTTPFields(const std::string &header);
		void			extractFile(const std::string &bound_data);
		void			bodyRequest(const std::string &body, size_t &total);
		void			bodyRequest(size_t index);
		void			quitAwaitingRequest();
		void			quitRequest();
		void			getErrorPage();
		int				setServBlock();


		int				awaitingHeader(int fd);
		void			awaitingBody(int fd);
		void			extractContentDisposition(const std::string &line, std::string &name, std::string &filename);
		void			extractContentType(const std::string &line, std::string &contentType);

		int				recvToBodyFile(int fd, std::ofstream &out);
		int				openBodyFile(std::ofstream &out);
		void			checkBodyBytesRecieved();
		void			addQueryString(const std::string &key, const std::string &value);

		int				pickServBlock();
		std::string		getRightHost(const std::string& host);
		std::string		getIPFromHostName(const std::string& hostName);

		std::string		getHostNameFromIP(const std::string& ipAddress);
		int				selectBlockWithServerName(std::vector<Server> vctServSelect, std::vector<int> index);

		void			checkCgiPath();


};

std::ostream &operator<<( std::ostream & o, Request const & rhs);

#endif