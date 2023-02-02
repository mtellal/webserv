#include "../includes/Response.hpp"
#include <sstream>
#include <string.h>
#include <unistd.h>

Response::Response() {}

Response::Response(Request req, std::vector<Server> vctServ) : _req(req), _vctServ(vctServ),
					_locBlocSelect(false){
	if (req.getMethod() == "GET")
	{
		Server serv = selectServerBlock();
		// Location loc = selectLocationBlock(serv);
		int loc = selectLocationBlock(serv);

		this->createResponse(serv ,loc);
	}
}

Response::Response(Response const &src) {
	*this = src;
}

Response::~Response() {}

Response	&Response::operator=(Response const &rhs) {
	if (this != &rhs)
	{
		this->_req = rhs._req;
		this->_vctServ = rhs._vctServ;
		this->_path = rhs._path;
	}
	return *this;
}

// std::vector<std::string>	Response::getRightPath(Server serv, Location loc) {
std::vector<std::string>	Response::getRightPath(Server serv, int loc) {
	std::vector<std::string>	res;
	std::string					root;
	std::string					newPath;
	Location					blocLoc = serv.getVctLocation()[loc];

	if (this->_locBlocSelect and blocLoc.getRootSet())
		root = blocLoc.getRoot();
	else if (serv.getRootSet())
		root = serv.getRoot();

	// std::cout << "ROOT = " << root << std::endl;


	// Remplacer par : si le path entree par l'utilisateur est un dossier
	if (this->_req.getPath()[this->_req.getPath().size() - 1] == '/')
	{
		// std::cout << "=====================================" << std::endl;
		std::vector<std::string>	index;

		root.erase(0, 1);

		if (this->_locBlocSelect and blocLoc.getIndexSet())
		{
			newPath = this->_req.getPath().erase(0, blocLoc.getPath().size());
			root += newPath;
			index = blocLoc.getIndex();
			for (size_t i = 0; i < index.size(); i++)
				res.push_back(index[i].insert(0, root));
		}
		else
		{
			root += "/";
			index = serv.getIndex();
			for (size_t i = 0; i < index.size(); i++)
				res.push_back(index[i].insert(0, root));
		}
		// for (size_t i = 0; i < res.size(); i++)
			// std::cout << res[i] << std::endl;
	}
	else
	{
		// Sinon, pas besoin de prendre en compte index
		if (this->_locBlocSelect and blocLoc.getIndexSet())
		{
			newPath = this->_req.getPath().erase(0, blocLoc.getPath().size());
			root += newPath;
			root.erase(0, 1);
			res.push_back(root);
			// std::cout << "A = " << root << std::endl;
		}
		else
		{
			newPath = this->_req.getPath();
			root += newPath;
			root.erase(0, 1);
			res.push_back(root);
			// std::cout << "B = " << root << std::endl;
		}
	}
	return res;
}


// void	Response::createResponse(Server serv, Location loc) {
void	Response::createResponse(Server serv, int loc) {
	std::string 				str;
	std::vector<std::string>	index;

	this->_path = this->getRightPath(serv, loc);

	// if (this->_req.getPath() == "/")
	// {
	// 	// std::cout << "A" << std::endl;
	// 	index = serv.getIndex();
	// 	for (size_t i = 0; i < index.size(); i++)
	// 	{
	// 		str += serv.getRoot();
	// 		str += this->_req.getPath();
	// 		str += index[i];
	// 		this->_path.push_back(str);
	// 		str.clear();
	// 	}
	// }
	// else
	// {
	// 	// std::cout << "B" << std::endl;
	// 	str += serv.getRoot();
	// 	str += this->_req.getPath();
	// 	this->_path.push_back(str);
	// }
	// for (size_t i = 0; i < this->_path.size(); i++)
		// std::cout << "path = " << this->_path[i] << std::endl;
	this->fileToStr();
}

void	Response::fileToStr() {
	size_t		i = 0;
	std::string	res;

	while (i < this->_path.size())
	{
		// std::cout << "path = " << this->_path[i] << std::endl;
		// this->_path[i].erase(0, 1);
		std::ifstream tmp(this->_path[i].c_str(), std::ios::in);

		if (tmp)
		{
			this->_statusCode = 200;
			tmp.close();
			break;
		}
		i++;
		if (i == this->_path.size())
		{
			this->_statusCode = 404;
			i--;
			this->_path[i] = "html/404.html";
			break ;
		}
	}

	Header	header("HTTP/1.1", this->_statusCode, this->_httpRep, this->_path[i]);
	res = header.getHeader();
	// // std::cout << res << std::endl;
	write(this->_req.getFd(), res.c_str(), res.length());

	std::ifstream file(this->_path[i].c_str());
	std::string str;
	if (file)
	{
		std::ostringstream ss;
		ss << file.rdbuf();
		str = ss.str();
	}
	write(this->_req.getFd(), str.c_str(), str.size());
	file.close();
}

Server	Response::selectServerBlock() {
	std::vector<Server>	tmp;
	std::vector<Server>	conf = this->_vctServ;
	bool				err = false;

	for (size_t i = 0; i < conf.size(); i++)
	{
		if (conf[i].getHost() == this->_req.getHost() and
				conf[i].getPort() == ft_stoi(this->_req.getPort(), &err))
			tmp.push_back(conf[i]);
	}
	if (tmp.size() == 1)
		return tmp[0];

	// Il faut egalement departager si besoin avec server_name
	// Si ce msg apparait, plusieurs blocs ont etes pre-selectionnes
	// mais pas departages.
	std::cout << "Fct selectServerBlock err" << std::endl;

	// for (size_t i = 0; i < tmp.size(); i++)
	// {
	// 	std::vector<std::string>	servName = tmp[i].getServerName();
	// 	for (size_t j = 0; j < servName.size(); j++)
	// 	{
	// 		if (servName[j] == this->_req.getHost())
	// 			return tmp[i];
	// 	}
	// }
	return tmp[0];
}

// Location	Response::selectLocationBlock(Server serv) {
int		Response::selectLocationBlock(Server serv) {
	std::vector<Location>	vctLoc = serv.getVctLocation();
	std::string				strBlocLoc;
	Location				tmp;
	int						res;
	std::string				req = this->_req.getPath();
	size_t j;

	for (size_t i = 0; i < vctLoc.size(); i++)
	{
		strBlocLoc = vctLoc[i].getPath();
		j = 0;
		while (strBlocLoc[j])
		{
			if (req[j] != strBlocLoc[j])
				break ;
			j++;
		}
		if (j > 2 and (!strBlocLoc[j] or (strBlocLoc[j] == '/' and !strBlocLoc[j + 1]))
			and (req[j] == '/' or !req[j]) and strBlocLoc.size() > tmp.getPath().size())
		{
			std::cout << "LOC = " << strBlocLoc << std::endl;
			this->_locBlocSelect = true;
			tmp = vctLoc[i];
			res = i;
		}
	}
	return res;
}