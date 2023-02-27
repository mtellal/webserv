#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <iostream>
# include <sys/socket.h>
# include <vector>
# include <map>
# include <fstream>

# define BUFFLEN 4096

class Request {

	public:
	
		Request();
		Request(int fd);
		Request(Request const &src);
		~Request();

		Request	&operator=(Request const &rhs);

		void								parsRequest(int fd);
		void								setErrorRequest(bool);

		bool								getErrRequest() const;
		bool								getcloseConnection() const;
		bool								getAwaitingRequest() const;
		bool								getEndAwaitingRequest() const; 
		bool								getConnectionSet() const;
		bool								getAcceptSet() const;
		bool								getRefererSet() const;
		bool								getAgentSet() const;
		bool								getBadRequest() const;
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
		std::map<std::string, std::string>	getQueryString() const;



	private:

		int									_fd;
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
		std::map<std::string, std::string>	_queryString;
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

		bool								_awaitingRequest;
		bool								_endAwaitingRequest;
		size_t								_bytesRecieved;
		bool								_bodyFileExists;
		std::string							_bodyFilePath;


		void			(Request::*functPtr[12])(std::vector<std::string>);
		void			parsArgs(std::string tmp);
		void			setMethodVersionPath(std::vector<std::string> strSplit);
		void			setHostPort(std::vector<std::string> strSplit);
		void			setConnection(std::vector<std::string> strSplit);
		void			setAccept(std::vector<std::string> strSplit);
		void			setReferer(std::vector<std::string> strSplit);
		void			setAgent(std::vector<std::string> strSplit);
		void			setAuthentification(std::vector<std::string> strSplit);
		void			setContentLength(std::vector<std::string> strSplit);
		void			setContentType(std::vector<std::string> strSplit);
		void			setGetParams(std::vector<std::string> vct, size_t *i);
		void			parseBodyFile();
		void			setHTTPFields(const std::string &header);
		void			parseBoundaryData(const std::string &bound_data);
		void			bodyRequest(const std::string &body, size_t &total);
		void			bodyRequest(int fd, char buff[BUFFLEN + 1],
										const std::string &body, size_t &body_bytes, size_t index);
		void			quitAwaitingRequest();
		void			quitRequest();
		void			getErrorPage();

		bool			postRequest(size_t total);
		int				awaitingRequest(int fd);
		std::string		extractFileName(const std::string &line);
};

std::ostream &operator<<( std::ostream & o, Request const & rhs);

#endif