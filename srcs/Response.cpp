#include "../includes/Response.hpp"
#include <sstream>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <features.h>
#include <dirent.h>


Response::Response() {}

Response::Response(Request req, std::vector<Server> vctServ, std::map<int, int> clientServer) : _req(req), _vctServ(vctServ),
					_clientServer(clientServer), _locBlocSelect(false), _isDir(false), _autoindex(false), _closeConnection(false) {
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
		this->_clientServer = rhs._clientServer;
		this->_path = rhs._path;
		this->_errPath = rhs._errPath;
		this->_httpRep = rhs._httpRep;
		this->_statusCode = rhs._statusCode;
		this->_locBlocSelect = rhs._locBlocSelect;
		this->_isDir = rhs._isDir;
		this->_autoindex = rhs._autoindex;
		this->_closeConnection = rhs._closeConnection;
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
	struct stat					fileOrDir;
	std::string					root = this->getRightRoot(serv, blocLoc);
	std::string					newPath;
	std::vector<std::string>	index;


	root.erase(0, 1);
	newPath = this->_req.getPath().erase(0, blocLoc.getPath().size());
	root += newPath;
	stat(root.c_str(), &fileOrDir);
	if (S_ISREG(fileOrDir.st_mode))
	{
		std::cout << "LOC FICHIER" << std::endl;
		this->_path.push_back(root);
	}
	else if (S_ISDIR(fileOrDir.st_mode))
	{
		std::cout << "LOC DOSSIER" << std::endl;
		this->_isDir = true;
		if (root[root.size() - 1] != '/')
			root += "/";
		index = blocLoc.getIndex();
		for (size_t i = 0; i < index.size(); i++)
			this->_path.push_back(index[i].insert(0, root));
		if (blocLoc.getAutoindexSet())
			this->_autoindex = blocLoc.getAutoindex();
		else if (serv.getAutoindexSet())
			this->_autoindex = serv.getAutoindex();
	}
	else
	{
		std::cout << "LOC ERR" << std::endl;
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
		std::cout << "PAS LOC FICHIER" << std::endl;
		this->_path.push_back(root);
	}
	else if (S_ISDIR(fileOrDir.st_mode))
	{
		std::cout << "PAS LOC DOSSIER" << std::endl;
		this->_isDir = true;
		if (root[root.size() - 1] != '/')
			root += "/";
		index = serv.getIndex();
		for (size_t i = 0; i < index.size(); i++)
			this->_path.push_back(index[i].insert(0, root));
		if (serv.getAutoindexSet())
			this->_autoindex = serv.getAutoindex();
	}
	else
	{
		std::cout << "PAS LOC ERR" << std::endl;
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

	// for (size_t j = 0; j < this->_path.size(); j++)
	// 	std::cout << this->_path[j] << std::endl;
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

void	Response::getFileAndDir(Server serv, std::ofstream &file, bool getDir, std::string path) {
	DIR				*dir;
	struct dirent	*entry;

	if ((dir = opendir(path.c_str())) != NULL)
	{
		while ((entry = readdir(dir)) != NULL)
		{
			if ((getDir and entry->d_type == DT_DIR) or (!getDir and entry->d_type != DT_DIR))
			{
				if (strlen(entry->d_name) != 1 or entry->d_name[0] != '.')
				{
					file << "<a href=\"http://" + serv.getHost() + ":" + ft_itos(serv.getPort()) +
					this->_req.getPath();
					if (this->_req.getPath()[this->_req.getPath().size() - 1] != '/')
						file << "/";
					file << entry->d_name << "\">" << entry->d_name;
					if (entry->d_type == DT_DIR)
						file << "/";
					file << "</a><br>" << std::endl;
					// std::cout << "<a href=\"http://" + serv.getHost() + ":" + ft_itos(serv.getPort()) +
					// this->_req.getPath();
					// if (this->_req.getPath()[this->_req.getPath().size() - 1] != '/')
					// 	std::cout << "/";
					// std::cout << entry->d_name << "\">" << entry->d_name;
					// if (entry->d_type == DT_DIR)
					// 	std::cout << "/";
					// std::cout << "</a><br>" << std::endl;
				}
			}
		}
		closedir(dir);
	} 
	else
		perror("Impossible d'ouvrir le répertoire");
}

std::string	Response::createAutoindexPage(Server serv) {
	std::string		path = this->_path[0];
	size_t			pos = path.find_last_of('/');
	std::ofstream	file("/tmp/tmpFile.html", std::ios::out | std::ios::trunc);

	path.erase(pos, path.size() - pos);
	path += "/";

	file << "<!DOCTYPE html>" << std::endl;
	file << "<html lang=\"en\">" << std::endl;
	file << "<head>" << std::endl;
	file << "	<meta charset=\"UTF-8\">" << std::endl;
	file << "	<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">" << std::endl;
	file << "	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
	file << "	<title>Webserv Autoindex</title>" << std::endl;
	file << "</head>" << std::endl;
	file << "<body>" << std::endl;
	file << "	<h1>Index of " + this->_req.getPath() + "</h1>" << std::endl;

	this->getFileAndDir(serv, file, true, path);
	this->getFileAndDir(serv, file, false, path);

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
		rightPath = this->testAllPaths(err);
	if (err)
	{
		bool	pageFind = false;

		if (this->_isDir)
			this->_statusCode = 403;
		else
			this->_statusCode = 404;

		rightPath = this->getRightPathErr(serv, blocLoc, pageFind);

		std::ifstream tmp(rightPath.c_str(), std::ios::in);

		if (this->_autoindex)
			rightPath = this->createAutoindexPage(serv);
		else if (!tmp or !pageFind)
			rightPath = this->createDefaultErrorPage();
		else
			tmp.close();
	}


	// Header	header("HTTP/1.1", this->_statusCode, this->_httpRep, rightPath);
	Header	header(this->_req, rightPath, this->_statusCode);
	res = header.getHeader();

	std::cout << res << std::endl;

	std::ifstream file(rightPath.c_str());
	std::string str;
	if (file)
	{
		std::ostringstream ss;
		ss << file.rdbuf();
		str = ss.str();
	}
	res += str;
	write(this->_req.getFd(), res.c_str(), res.size());
	file.close();

	if (this->_req.getConnection() == "close")
		this->_closeConnection = true;
}

Server	Response::selectServerBlock() {
	std::vector<Server>			tmp;
	std::string					host;
	std::vector<Server>			conf = this->_vctServ;
	std::vector<std::string>	servName;
	bool						err = false;
	int							fd = this->_clientServer[this->_req.getFd()];

	host = conf[fd].getHost();
	// std::cout << host << std::endl;


	// On check si il y a une correspondance parfaite entre host et port
	for (size_t i = 0; i < conf.size(); i++)
	{
		if (conf[i].getHost() == host and
				conf[i].getPort() == ft_stoi(this->_req.getPort(), &err))
			tmp.push_back(conf[i]);
	}
	// std::cout << "SIZE " << tmp.size() << std::endl;
	if (tmp.size() == 1)
		return tmp[0];
	else if (tmp.size() > 1)
		std::cout << "Fct selectServerBlock err, plusieurs blocs preselectionnes" << std::endl;
	// else
	// {
	// 	for (size_t i = 0; i < conf.size(); i++)
	// 	{
	// 		if (conf[i].getPort() == ft_stoi(this->_req.getPort(), &err) and conf[i].getHost() == "0.0.0.0")
	// 			tmp.push_back(conf[i]);
	// 	}
	// 	if (tmp.size() == 1)
	// 		return tmp[0];
	// 	else
	// 	{
	// 		for (size_t i = 0; i < tmp.size(); i++)
	// 		{
	// 			if (tmp[i].getServerNameSet())
	// 			{
	// 				servName = tmp[i].getServerName();
	// 				for (size_t j = 0; j < servName.size(); j++)
	// 				{
	// 					if (servName[j] == this->_req.getHost())
	// 						return tmp[i];
	// 				}
	// 			}
	// 		}
	// 		return tmp[0];
	// 	}
	// }
	// Il faut egalement departager si besoin avec server_name

	// Si ce msg apparait, plusieurs blocs ont etes pre-selectionnes
	// mais pas departages, il faut les departages avec server name.
	std::cout << "Fct selectServerBlock err" << std::endl;
	return tmp[0];
	// return conf[0];
}

int		Response::selectLocationBlock(Server serv) {
	std::vector<Location>	vctLoc = serv.getVctLocation();
	std::string 			strBlocLoc;
	Location				tmp;
	int						res;
	std::string				req = this->_req.getPath();
	size_t j;

	// std::cout << "req = " << req << std::endl;
	for (size_t i = 0; i < vctLoc.size(); i++)
	{
		strBlocLoc = vctLoc[i].getPath();
		// std::cout << "strBlocLoc = " << strBlocLoc << std::endl;
		if (strBlocLoc == "*.php" and req.size() > 4 and
			!req.compare(req.size() - 4, 4, ".php"))
		{
			// std::cout << "PHP LOC = " << strBlocLoc << std::endl;
			this->_locBlocSelect = true;
			tmp = vctLoc[i];
			// res = i;
			return i;
		}
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
			// std::cout << "LOC = " << strBlocLoc << std::endl;
			this->_locBlocSelect = true;
			tmp = vctLoc[i];
			res = i;
		}
	}
	return res;
}

bool		Response::getCloseConnection() const {
	return this->_closeConnection;
}
