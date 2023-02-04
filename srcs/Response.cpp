#include "../includes/Response.hpp"
#include <sstream>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <features.h>

Response::Response() {}

Response::Response(Request req, std::vector<Server> vctServ) : _req(req), _vctServ(vctServ),
					_locBlocSelect(false), _isDir(false) {
	// if (req.getMethod() == "GET")
	// {
		Server serv = selectServerBlock();
		int loc = selectLocationBlock(serv);

		this->fileToStr(serv ,loc);
	// }
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

std::string	Response::getRightRoot(Server &serv, Location &blocLoc) {
	std::string	root;

	if (this->_locBlocSelect and blocLoc.getRootSet())
		root = blocLoc.getRoot();
	else if (serv.getRootSet())
		root = serv.getRoot();
	return root;
}

void	Response::getRightPathLocation(Server serv, Location &blocLoc, bool &res) {
	struct stat	fileOrDir;
	std::string	root = this->getRightRoot(serv, blocLoc);
	std::string	newPath;
	std::vector<std::string>	index;


	root.erase(0, 1);
	newPath = this->_req.getPath().erase(0, blocLoc.getPath().size());
	root += newPath;
	stat(root.c_str(), &fileOrDir);
	if (S_ISREG(fileOrDir.st_mode))
	{
		// std::cout << "LOC FICHIER" << std::endl;
		this->_path.push_back(root);
	}
	else if (S_ISDIR(fileOrDir.st_mode))
	{
		// std::cout << "LOC DOSSIER" << std::endl;
		this->_isDir = true;
		index = blocLoc.getIndex();
		for (size_t i = 0; i < index.size(); i++)
			this->_path.push_back(index[i].insert(0, root));
	}
	else
	{
		// std::cout << "LOC ERR" << std::endl;
		res = true;
	}
}

void	Response::getRightPathServer(Server serv, Location &blocLoc, bool &res) {
	struct stat					fileOrDir;
	std::string					root = this->getRightRoot(serv, blocLoc);
	std::string					newPath;
	std::vector<std::string>	index;

	root.erase(0, 1);
	newPath = this->_req.getPath();
	root += newPath;
	// std::cout << "ROOT_BIS = " << root << std::endl;
	stat(root.c_str(), &fileOrDir);
	if (S_ISREG(fileOrDir.st_mode))
	{
		// std::cout << "PAS LOC FICHIER" << std::endl;
		this->_path.push_back(root);
	}
	else if (S_ISDIR(fileOrDir.st_mode))
	{
		// std::cout << "PAS LOC DOSSIER" << std::endl;
		this->_isDir = true;
		if (root[root.size() - 1] != '/')
			root += "/";
		index = serv.getIndex();
		for (size_t i = 0; i < index.size(); i++)
			this->_path.push_back(index[i].insert(0, root));
	}
	else
	{
		// std::cout << "PAS LOC ERR" << std::endl;
		res = true;
	}
}


bool	Response::getRightPath(Server serv, int loc) {
	Location					blocLoc = serv.getVctLocation()[loc];
	bool						res = false;

	if (this->_locBlocSelect and blocLoc.getIndexSet())
		this->getRightPathLocation(serv, blocLoc, res);
	else
		this->getRightPathServer(serv, blocLoc, res);

	// for (size_t i = 0; i < this->_path.size(); i++)
		// std::cout << this->_path[i] << std::endl;
	return res;
}

std::string	Response::testAllPaths(bool &err) {
	size_t		i = 0;
	std::string	rightPath;

	while (i < this->_path.size())
	{
		std::ifstream tmp(this->_path[i].c_str(), std::ios::in);

		if (tmp)
		{
			rightPath = this->_path[i];
			this->_statusCode = 200;
			tmp.close();
			break;
		}
		i++;
		if (i == this->_path.size())
		{
			// std::cout << "BREAKKKKKKKKKKKKK" << std::endl;
			err = true;
			break ;
		}
	}
	return rightPath;
}

std::string	Response::getRightPathErr(Server serv, Location &blocLoc, bool &pageFind) {
	std::string									root = getRightRoot(serv, blocLoc);
	std::map<int, std::string>					mapErr;
	std::map<int, std::string>::const_iterator	it;
	std::string									rightPath;

	if (this->_locBlocSelect and blocLoc.getErrorPageSet())
	{
		mapErr = blocLoc.getErrorPage();
		it = mapErr.find(this->_statusCode);
		if (it != mapErr.end())
		{
			pageFind = true;
			rightPath = it->second;
			root = blocLoc.getRoot();
			root.erase(0, 1);
			root += "/";
			root += rightPath;
			rightPath = root;
		}
	}
	if (!pageFind and it != mapErr.end())
	{
		mapErr = serv.getErrorPage();
		it = mapErr.find(this->_statusCode);
		if (it != mapErr.end())
		{
			pageFind = true;
			rightPath = it->second;
			root = serv.getRoot();
			root.erase(0, 1);
			root += "/";
			root += rightPath;
			rightPath = root;
		}
	}
	return rightPath;
}

std::string	Response::createDefaultErrorPage() {
	std::ofstream file("/tmp/tmpFile.html", std::ios::out | std::ios::trunc);

	file << "<!DOCTYPE html>" << std::endl;
	file << "<html lang=\"en\">" << std::endl;
	file << "<head>" << std::endl;
	file << "	<meta charset=\"UTF-8\">" << std::endl;
	file << "	<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">" << std::endl;
	file << "	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
	file << "	<title>TMP Webserv " + ft_itos(this->_statusCode) + "</title>" << std::endl;
	file << "</head>" << std::endl;
	file << "<body>" << std::endl;
	file << "	<h1>TMP Error " + ft_itos(this->_statusCode) + " BIS :(</h1>" << std::endl;
	file << "</body>" << std::endl;
	file << "</html>" << std::endl;
	file.close();
	return "/tmp/tmpFile.html";
}


void	Response::fileToStr(Server serv, int loc) {
	std::string	res;
	std::string	rightPath;
	Location	blocLoc = serv.getVctLocation()[loc];
	bool		err = false;

	if (!(err = this->getRightPath(serv, loc)))
		rightPath = testAllPaths(err);
	if (err)
	{
		bool	pageFind = false;

		if (this->_isDir)
			this->_statusCode = 403;
		else
			this->_statusCode = 404;

		rightPath = getRightPathErr(serv, blocLoc, pageFind);

		std::ifstream tmp(rightPath.c_str(), std::ios::in);

		if (!tmp or !pageFind)
			rightPath = createDefaultErrorPage();
		else
			tmp.close();
	}



	Header	header("HTTP/1.1", this->_statusCode, this->_httpRep, rightPath);
	res = header.getHeader();
	// std::cout << res << std::endl;
	write(this->_req.getFd(), res.c_str(), res.length());

	std::ifstream file(rightPath.c_str());
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
	// mais pas departages, il faut les departages avec server name.
	std::cout << "Fct selectServerBlock err" << std::endl;
	return tmp[0];
}

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