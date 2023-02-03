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

	// void						createResponse(Server serv, Location loc);
	// void						createResponse(Server serv, int loc);
	void						fileToStr(Server serv, int loc);
	Server						selectServerBlock();
	// Location					selectLocationBlock(Server serv);
	int							selectLocationBlock(Server serv);
	// std::vector<std::string>	getRightPath(Server serv, Location loc);
	bool						getRightPath(Server serv, int loc);
};

#endif