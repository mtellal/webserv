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

class Response
{

	public:
		
		Response();
		Response(const Request &req, const Server &s, char **envp);
		Response(Response const &src);
		~Response();

		Response	&operator=(Response const &rhs);

		void						sendData();
		bool						getlocBlocSelect() const;
		bool						getCloseConnection() const;
		Server						getServ() const;
		Location					getLocBloc() const;
		Request						getRequest() const;


	private:

		bool						_isDir;
		bool						_autoindex;
		bool						_locBlocSelect;
		bool						_closeConnection;
		char						**_envp;
		int							_statusCode;

		std::string					_httpRep;
		Request						_req;
		Server						_serv;
		std::ifstream				_file;
		Location					_locBloc;
		DefaultPage					_defaultPage;
		std::vector<std::string>	_path;
		std::vector<std::string>	_errPath;

		void						httpRedir();
		void						printResponse() const;
		void						findRightCodeError();
		void						sendHeader(std::string path);
		void						sendPage(std::string path, const std::string &cgi_content);
		bool						rightPath();
		bool						rightPathServer();
		bool						rightPathLocation();
		bool						methodNotAllowed() const;
		std::string					findRightError();
		std::string					deleteResource();
		std::string					sendContentTypeError();
		std::string					testAllPaths(bool *err);
		std::vector<std::string>	rightIndex();
};


std::ostream	&operator<<(std::ostream &out, const Response &res);

#endif