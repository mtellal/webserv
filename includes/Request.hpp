#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <iostream>
# include <sys/socket.h>
# include <vector>
# include <map>

class Request {

	public:
	Request();
	Request(int fd);
	Request(Request const &src);
	~Request();

	Request	&operator=(Request const &rhs);

	int			getFd() const;
	bool		getErrRequest() const;
	bool		getcloseConnection() const;
	std::string	getMethod() const;
	std::string	getPath() const;
	std::string	getHttpVersion() const;
	std::string	getHost() const;
	std::string	getPort() const;

	private:
	int									_fd;
	bool								_errRequest;
	bool								_parsArgsGet;
	bool								_closeConnection;
	std::string							_method;
	std::string							_path;
	std::string							_httpVersion;
	std::string							_host;
	std::string							_port;
	std::map<std::string, std::string>	_argsGet;

	int			parsRequest(int fd);


};

std::ostream &operator<<( std::ostream & o, Request const & rhs);

#endif