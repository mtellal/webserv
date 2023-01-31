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
	std::ifstream				_file;
	std::string					_httpRep;
	int							_statusCode;
	bool						_locBlocSelect;

	void		createResponse(Server serv);
	void		fileToStr();
	Server		selectServerBlock();
	Location	selectLocationBlock(Server serv);
};

#endif