#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "../includes/Request.hpp"
# include "../includes/Server.hpp"
# include "../includes/Header.hpp"

class Response {

	public:
	Response();
	Response(Request req, std::vector<Server> vctServ, std::map<int, int> clientServer);
	Response(Response const &src);
	~Response();

	Response	&operator=(Response const &rhs);

	Server		getServ() const;
	bool		getlocBlocSelect() const;
	Location	getLocBloc() const;
	bool		getCloseConnection() const;

	private:
	Server						_serv;
	Request						_req;
	std::vector<Server>			_vctServ;
	std::map<int, int>			_clientServer;
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

	std::string					rightRoot();
	std::vector<std::string>	rightIndex();
	void						rightPathLocation(bool &res);
	void						rightPathServer(bool &res);
	std::string					rightPathErr(bool &pageFind);
	void						fileAndDir(std::ofstream &file, bool getDir, std::string path);


	std::string	testAllPaths(bool &err);
	std::string	createDefaultErrorPage();
	std::string	createAutoindexPage();
	std::string	createResFormPage();
	// std::string	argsToStr();
	void		checkError();
	Server		selectServerBlock();
	void		selectLocationBlock();
	bool		rightPath();
	void		sendHeader(std::string path);
	void		sendPage(std::string path);
};

#endif