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
					_clientServer(clientServer), _locBlocSelect(false), _isDir(false), _autoindex(false),
					_closeConnection(false), _isResFormPage(false) {

		this->_serv = selectServerBlock();
		this->selectLocationBlock();
		this->checkError();
}

Response::Response(Response const &src) {
	*this = src;
}

Response::~Response() {}

Response	&Response::operator=(Response const &rhs) {
	if (this != &rhs)
	{
		this->_serv = rhs._serv;
		this->_req = rhs._req;
		this->_vctServ = rhs._vctServ;
		this->_clientServer = rhs._clientServer;
		this->_path = rhs._path;
		this->_errPath = rhs._errPath;
		this->_httpRep = rhs._httpRep;
		this->_statusCode = rhs._statusCode;
		this->_locBlocSelect = rhs._locBlocSelect;
		this->_locBloc = rhs._locBloc;
		this->_isDir = rhs._isDir;
		this->_autoindex = rhs._autoindex;
		this->_closeConnection = rhs._closeConnection;
		this->_isResFormPage = rhs._isResFormPage;
	}
	return *this;
}

Server		Response::getServ() const {
	return this->_serv;
}

bool		Response::getlocBlocSelect() const {
	return this->_locBlocSelect;
}

Location	Response::getLocBloc() const {
	return this->_locBloc;
}

bool		Response::getCloseConnection() const {
	return this->_closeConnection;
}

std::string	Response::rightRoot() {
	std::string	root;

	if (this->_locBlocSelect and this->_locBloc.getRootSet())
		root = this->_locBloc.getRoot();
	else
		root = this->_serv.getRoot();

	return root;
}

std::vector<std::string>	Response::rightIndex() {
	std::vector<std::string>	index;

	if (this->_locBloc.getIndexSet())
		index = this->_locBloc.getIndex();
	else
		index = this->_serv.getIndex();

	return index;
}

void	Response::rightPathLocation(bool &res) {
	struct stat					fileOrDir;
	std::string					root = this->rightRoot();
	std::string					newPath;
	std::vector<std::string>	index = this->rightIndex();

	if (root[0] == '/')
		root.erase(0, 1);
	newPath = this->_req.getPath().erase(0, this->_locBloc.getPath().size());
	root += newPath;
	stat(root.c_str(), &fileOrDir);
	if (S_ISREG(fileOrDir.st_mode))
		this->_path.push_back(root);
	else if (S_ISDIR(fileOrDir.st_mode))
	{
		this->_isDir = true;
		if (root[root.size() - 1] != '/')
			root += "/";
		for (size_t i = 0; i < index.size(); i++)
			this->_path.push_back(index[i].insert(0, root));
		if (this->_locBloc.getAutoindexSet())
			this->_autoindex = this->_locBloc.getAutoindex();
		else if (this->_serv.getAutoindexSet())
			this->_autoindex = this->_serv.getAutoindex();
	}
	else
		res = true;
}

void	Response::rightPathServer(bool &res) {
	struct stat					fileOrDir;
	std::string					root = this->rightRoot();
	std::string					newPath;
	std::vector<std::string>	index;

	if (root[0] == '/')
		root.erase(0, 1);
	newPath = this->_req.getPath();
	// std::cout << "newPath = " << newPath << std::endl;
	if (newPath == "/resForm.html")
	{
		// std::cout << "OK" << newPath << std::endl;
		res = true;
		this->_isResFormPage = true;
		return ;
	}
	root += newPath;
	// std::cout << "ROOT = " << root << std::endl;
	stat(root.c_str(), &fileOrDir);
	if (S_ISREG(fileOrDir.st_mode))
		this->_path.push_back(root);
	else if (S_ISDIR(fileOrDir.st_mode))
	{
		this->_isDir = true;
		if (root[root.size() - 1] != '/')
			root += "/";
		index = this->_serv.getIndex();
		for (size_t i = 0; i < index.size(); i++)
			this->_path.push_back(index[i].insert(0, root));
		if (this->_serv.getAutoindexSet())
			this->_autoindex = this->_serv.getAutoindex();
	}
	else
		res = true;
}


bool	Response::rightPath() {
	bool	res = false;

	if (this->_locBlocSelect)
		this->rightPathLocation(res);
	else
		this->rightPathServer(res);

	return res;
}

std::string	Response::testAllPaths(bool &err) {
	size_t		i = 0;
	std::string	rightPath;

	while (i < this->_path.size())
	{
		std::ifstream tmp(this->_path[i].c_str(), std::ios::in | std::ios::binary);

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
			err = true;
			break ;
		}
	}
	return rightPath;
}

std::string	Response::rightPathErr(bool &pageFind) {
	std::string									root = rightRoot();
	std::map<int, std::string>					mapErr;
	std::map<int, std::string>::const_iterator	it;
	std::string									rightPath;

	if (this->_locBlocSelect and this->_locBloc.getErrorPageSet())
	{
		mapErr = this->_locBloc.getErrorPage();
		it = mapErr.find(this->_statusCode);
		if (it != mapErr.end())
		{
			pageFind = true;
			rightPath = it->second;
			root = this->_locBloc.getRoot();
			if (root[0] == '/')
				root.erase(0, 1);
			if (root[root.size() - 1] != '/')
				root += "/";
			root += rightPath;
			rightPath = root;
		}
	}
	if (!pageFind and it != mapErr.end())
	{
		mapErr = this->_serv.getErrorPage();
		it = mapErr.find(this->_statusCode);
		if (it != mapErr.end())
		{
			pageFind = true;
			rightPath = it->second;
			root = this->_serv.getRoot();
			if (root[0] == '/')
				root.erase(0, 1);
			if (root[root.size() - 1] != '/')
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
	file << "	<h1>Default page Error " + ft_itos(this->_statusCode) + " :(</h1>" << std::endl;
	file << "</body>" << std::endl;
	file << "</html>" << std::endl;
	file.close();

	return "/tmp/tmpFile.html";
}

void	Response::fileAndDir(std::ofstream &file, bool getDir, std::string path) {
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
					file << "<a href=\"http://" + this->_serv.getHost() + ":" + ft_itos(this->_serv.getPort()) +
					this->_req.getPath();
					if (this->_req.getPath()[this->_req.getPath().size() - 1] != '/')
						file << "/";
					file << entry->d_name << "\">" << entry->d_name;
					if (entry->d_type == DT_DIR)
						file << "/";
					file << "</a><br>" << std::endl;
				}
			}
		}
		closedir(dir);
	} 
	else
		perror("Impossible d'ouvrir le répertoire");
}

std::string	Response::createAutoindexPage() {
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

	this->fileAndDir(file, true, path);
	this->fileAndDir(file, false, path);

	file << "</body>" << std::endl;
	file << "</html>" << std::endl;
	file.close();

	return "/tmp/tmpFile.html";
}

std::string	Response::argsToStr() {
	std::map<std::string, std::string>	args;
	std::string							res;

	args = this->_req.getQueryString();

	res += args["titre"];
	res += " ";
	res += args["nom"];
	res += " ";
	res += args["prenom"];
	res += " a ";
	res += args["age"];
	res += " ans, ";
	if (args["titre"] == "M.")
		res += "il ";
	else
		res += "elle ";
	if (args["bDebutant"] == "on")
		res += "debute en php";
	else
		res += "est un pro en php";

	return res;
}



std::string	Response::createResFormPage() {
	std::ofstream file("/tmp/tmpFile.html", std::ios::out | std::ios::trunc);

	file << "<!DOCTYPE html>" << std::endl;
	file << "<html lang=\"en\">" << std::endl;
	file << "<head>" << std::endl;
	file << "	<meta charset=\"UTF-8\">" << std::endl;
	file << "	<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">" << std::endl;
	file << "	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
	file << "	<title>Form</title>" << std::endl;
	file << "</head>" << std::endl;
	file << "<body>" << std::endl;
	file << "	<p>" + this->argsToStr() + "</p>"<< std::endl;
	file << "</body>" << std::endl;
	file << "</html>" << std::endl;
	file.close();

	return "/tmp/tmpFile.html";
}


void	Response::checkError() {
	std::string	res;
	std::string	path;
	bool		err = false;

	if (!(err = this->rightPath()))
		path = this->testAllPaths(err);
	if (err)
	{
		bool	pageFind = false;

		if (this->_isResFormPage)
		{
			// std::cout << "OKKKKK" << std::endl;
			path = this->createResFormPage();
			this->_statusCode = 200;
			pageFind = true;
		}
		else
		{
			if (this->_isDir)
				this->_statusCode = 403;
			else
				this->_statusCode = 404;

			path = this->rightPathErr(pageFind);
		}

		std::ifstream tmp(path.c_str(), std::ios::in | std::ios::binary);

		if (this->_autoindex)
			path = this->createAutoindexPage();
		else if (!tmp or !pageFind)
			path = this->createDefaultErrorPage();
		else
			tmp.close();
	}
	this->sendHeader(path);
}

void	Response::sendHeader(std::string path) {
	Header		header(this->_req, path, &this->_statusCode, this->_serv, this);
	std::string	res;

	res = header.getHeader();

	// std::cout << res << std::endl;

	if (this->_statusCode == 406)
	{
		path = this->createDefaultErrorPage();
		Header	headerBis(this->_req, path, &this->_statusCode, this->_serv, this);
		res = header.getHeader();
	}
	write(this->_req.getFd(), res.c_str(), res.size());
	this->sendPage(path);
	// std::cout << res << std::endl;
}

void	Response::sendPage(std::string path) {
	std::ifstream	file(path.c_str(), std::ios::in | std::ios::binary);
	std::string		str;
	std::string		res;

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
	bool						err = false;
	int							fd = this->_clientServer[this->_req.getFd()];

	host = conf[fd].getHost();

	for (size_t i = 0; i < conf.size(); i++)
	{
		if (conf[i].getHost() == host and
				conf[i].getPort() == ft_stoi(this->_req.getPort(), &err))
			tmp.push_back(conf[i]);
	}
	if (tmp.size() == 1)
		return tmp[0];
	else if (tmp.size() > 1)
		std::cout << "Fct selectServerBlock err, plusieurs blocs preselectionnes" << std::endl;

	// Il faut egalement departager si besoin avec server_name

	// Si ce msg apparait, plusieurs blocs ont etes pre-selectionnes
	// mais pas departages, il faut les departages avec server name.
	std::cout << "Fct selectServerBlock err" << std::endl;
	return tmp[0];
}

void	Response::selectLocationBlock() {
	std::vector<Location>	vctLoc = this->_serv.getVctLocation();
	std::string 			strBlocLoc;
	Location				tmp;
	// int						res;
	std::string				req = this->_req.getPath();
	size_t j;

	for (size_t i = 0; i < vctLoc.size(); i++)
	{
		strBlocLoc = vctLoc[i].getPath();
		if (strBlocLoc == "*.php" and req.size() > 4 and
			!req.compare(req.size() - 4, 4, ".php"))
		{
			this->_locBlocSelect = true;
			this->_locBloc = vctLoc[i];
			return ;
		}
		j = 0;
		while (strBlocLoc[j])
		{
			if (req[j] != strBlocLoc[j])
				break ;
			j++;
		}
		if ((j > 2 and (!strBlocLoc[j] or (strBlocLoc[j] == '/' and !strBlocLoc[j + 1]))
			and (req[j] == '/' or !req[j]) and strBlocLoc.size() > tmp.getPath().size()))
		{
			this->_locBlocSelect = true;
			tmp = vctLoc[i];
		}
	}
	if (this->_locBlocSelect)
		this->_locBloc = tmp;
}
