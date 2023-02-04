#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "../includes/Request.hpp"
# include "../includes/Server.hpp"
# include "../includes/Header.hpp"

class Response {

	public:
	Response();
	Response(Request req, std::vector<Server> vctServ);
	Response(Response const &src);
	~Response();

	Response	&operator=(Response const &rhs);

	private:
	Request						_req;
	// Header						_header;
	std::vector<Server>			_vctServ;
	std::vector<std::string>	_path;
	std::vector<std::string>	_errPath;
	std::ifstream				_file;
	std::string					_httpRep;
	int							_statusCode;
	bool						_locBlocSelect;
	bool						_isDir;

	std::string	getRightRoot(Server &serv, Location &blocLoc);
	void		getRightPathLocation(Server serv, Location &blocLoc, bool &res);
	void		getRightPathServer(Server serv, Location &blocLoc, bool &res);
	std::string	getRightPathErr(Server serv, Location &blocLoc, bool &pageFind);


	std::string	testAllPaths(bool &err);
	std::string	createDefaultErrorPage();
	void		fileToStr(Server serv, int loc);
	Server		selectServerBlock();
	int			selectLocationBlock(Server serv);
	bool		getRightPath(Server serv, int loc);
};

#endif