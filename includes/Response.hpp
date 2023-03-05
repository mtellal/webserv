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

		void						selectServerBlock();
		void						selectLocationBlock();
		void						sendData();
		bool						getlocBlocSelect() const;
		bool						getCloseConnection() const;
		Server						getServ() const;
		Request						getRequest() const;
		Location					getLocBloc() const;


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
		char						**_envp;
		std::string					cgi_script;
		DefaultPage					_defaultPage;

		bool						rightPathLocation();
		bool						rightPathServer();
		std::string					rightPathErr(bool &pageFind);
		std::string					rightRoot();
		std::vector<std::string>	rightIndex();

		void						sendHeader(std::string path);
		void						sendPage(std::string path, const std::string &cgi_content);
		void						httpRedir();
		void						findRightCodeError();
		bool						rightPath();
		bool						methodNotAllowed() const;
		std::string					sendContentTypeError();
		std::string					findRightError();
		std::string					testAllPaths(bool *err);
		std::string					deleteResource();
		void						printStatusCode();
};


std::ostream	&operator<<(std::ostream &out, const Response &res);

#endif