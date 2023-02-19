#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <sstream>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <features.h>
#include <dirent.h>

# include "Request.hpp"
# include "Server.hpp"
# include "Header.hpp"
# include "DefaultPage.hpp"

class Response {

	public:
	Response();
	Response(const Request &req, const Server &s, char **envp);
	Response(Response const &src);
	~Response();

	Response	&operator=(Response const &rhs);

	Server		getServ() const;
	bool		getlocBlocSelect() const;
	Location	getLocBloc() const;
	bool		getCloseConnection() const;

	void		selectServerBlock();
	void		selectLocationBlock();
	void		sendData();

	private:

	Server						_serv;
	Request						_req;
	std::vector<std::string>	_path;
	std::vector<std::string>	_errPath;
	std::ifstream				_file;
	std::string					_httpRep;
	int							_statusCode;
	bool						_locBlocSelect;
	Location					_locBloc;
	bool						_isDir;
	bool						_autoindex;
	bool						_closeConnection;
	bool						_isResFormPage;
	char						**_envp;
	std::string					cgi_script;

	DefaultPage					_defaultPage;


	std::string					rightRoot();
	std::vector<std::string>	rightIndex();
	void						rightPathLocation(bool *err);
	void						rightPathServer(bool *err);
	std::string					rightPathErr(bool &pageFind);
	void						fileAndDir(std::ofstream &file, bool getDir, std::string path);


	std::string	testAllPaths(bool *err);
	std::string	createDefaultErrorPage();
	std::string	createAutoindexPage();
	std::string	createResFormPage();
	std::string	argsToStr();
	bool		rightPath();
	void		sendHeader(std::string path);
	void		sendPage(std::string path);
};


std::ostream	&operator<<(std::ostream &out, const Response &res);

#endif