#include "Response.hpp"
#include "Cgi.hpp"
#include <stdio.h>
#include <cstdlib>
#include <signal.h>

Response::Response() :
_isDir(false), _autoindex(false),
_locBlocSelect(false), _closeConnection(false) {} 

Response::Response(const Request &req, const Server &s, char **envp) :
_isDir(false), _autoindex(false),
_locBlocSelect(false), _closeConnection(false), 
_envp(envp), _req(req), _serv(s), _defaultPage(_req, _serv) {
	this->_locBlocSelect = req.getLocBlocSelect();
	if (this->_locBlocSelect)
		this->_locBloc = req.getLocationBlock();
}

Response::Response(Response const &src) { *this = src; }

Response::~Response() {}

Response	&Response::operator=(Response const &rhs) {
	if (this != &rhs)
	{
		this->_isDir = rhs._isDir;
		this->_autoindex = rhs._autoindex;
		this->_locBlocSelect = rhs._locBlocSelect;
		this->_closeConnection = rhs._closeConnection;
		this->_envp = rhs._envp;
		this->_statusCode = rhs._statusCode;
		this->_httpRep = rhs._httpRep;
		this->_req = rhs._req;
		this->_serv = rhs._serv;
		this->_locBloc = rhs._locBloc;
		this->_defaultPage = rhs._defaultPage;
		this->_path = rhs._path;
		this->_errPath = rhs._errPath;
	}
	return *this;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//												G E T T E R													  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool		Response::getlocBlocSelect() const {	return this->_locBlocSelect; }

bool		Response::getCloseConnection() const { return this->_closeConnection; }

Server		Response::getServ() const { return this->_serv; }

Request		Response::getRequest() const { return this->_req; }

Location	Response::getLocBloc() const { return this->_locBloc; }


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										M E M B E R S   F U N C T I O N S 									  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


std::vector<std::string>	Response::rightIndex() {
	std::vector<std::string>	index;

	if (this->_locBloc.getIndexSet())
		index = this->_locBloc.getIndex();
	else
		index = this->_serv.getIndex();

	return index;
}

/*	Reconstitue le chemin complet du fichier avec la directive root + 
	l'url qu'a entre l'utilisateur. Une fois le chemin reconstitue :
	- Si c'est un fichier, on l'ajoute a notre vct de path.
	- Si c'est un dossier, on ajoute un a un a notre vct de path, le path
	des fichiers indiques dans la directive index du fichier de conf
	et on regardera plus tard si ces fichiers existent ou pas. On regarde aussi si l'
	autoindex est "on" pour pouvoir l'afficher en cas de besoin.
	- Sinon, on met notre bool a true (donc erreur) */
bool	Response::rightPathLocation() {
	struct stat					fileOrDir;
	std::string					root = rightRoot(this->_serv, this->_locBlocSelect, this->_locBloc);
	std::string					newPath;
	std::vector<std::string>	index = this->rightIndex();

	memset(&fileOrDir, 0, sizeof(fileOrDir));
	if (root[0] == '/')
		root.erase(0, 1);
	newPath = this->_req.getPath().erase(0, this->_locBloc.getPath().size());
	if (root.size() > 0 && root[root.size() - 1] != '/' &&
		newPath.size() > 0 && newPath[0] != '/')
		root += '/';
	root += newPath;
	if (stat(root.c_str(), &fileOrDir) != -1)
	{
		if (S_ISREG(fileOrDir.st_mode) || this->_req.getMethod() == "DELETE")
			this->_path.push_back(root);
		else if (S_ISDIR(fileOrDir.st_mode))
		{
			this->_path.push_back(root);
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
			return true;
	}
	else
		return true;
	return false;
}

/*	Pareil que au dessus mais si un aucun bloc de Location est selectionne */
bool	Response::rightPathServer() {
	struct stat					fileOrDir;
	std::string					root = this->_serv.getRoot();
	std::string					newPath;
	std::vector<std::string>	index;

	memset(&fileOrDir, 0, sizeof(fileOrDir));
	if (root[0] == '/')
		root.erase(0, 1);
	newPath = this->_req.getPath();
	if (root.size() > 0 && root[root.size() - 1] != '/' &&
		newPath.size() > 0 && newPath[0] != '/')
		root += '/';
	root += newPath;
	if (stat(root.c_str(), &fileOrDir) != -1)
	{
		if (S_ISREG(fileOrDir.st_mode) || this->_req.getMethod() == "DELETE")
			this->_path.push_back(root);
		else if (S_ISDIR(fileOrDir.st_mode))
		{
			this->_path.push_back(root);
			if (root[root.size() - 1] != '/')
				root += "/";
			index = this->_serv.getIndex();
			for (size_t i = 0; i < index.size(); i++)
				this->_path.push_back(index[i].insert(0, root));
			if (this->_serv.getAutoindexSet())
				this->_autoindex = this->_serv.getAutoindex();
		}
		else
			return true;
	}
	else
		return true;
	return false;
}

bool	Response::rightPath() {
	bool	err;

	if (this->_locBlocSelect)
		err = this->rightPathLocation();
	else
		err = this->rightPathServer();

	return err;
}

/*	On va regarder parmis tous les paths selectionnes si au moins un existe, si c'est le cas,
	on renvoie le bon path sinon, on met notre bool err a true. */
std::string	Response::testAllPaths(bool *err) {
	size_t		i = 0;
	std::string	rightPath;
	struct stat	file;

	memset(&file, 0, sizeof(file));
	while (i < this->_path.size())
	{
		stat(this->_path[i].c_str(), &file);
		if (S_ISREG(file.st_mode))
		{
			std::ifstream tmp(this->_path[i].c_str(), std::ios::in | std::ios::binary);

			if (!tmp)
			{
				this->_isDir = true;
				*err = true;
				this->_statusCode = 403;
			}
			else
			{
				rightPath = this->_path[i];
				*err = false;
				this->_statusCode = 200;
				break;
			}
		}
		else if (S_ISDIR(file.st_mode))
		{
				this->_isDir = true;
				this->_statusCode = 403;
				*err = true;
		}
		i++;
		if (i == this->_path.size())
		{
			*err = true;
			break ;
		}
	}
	return rightPath;
}

bool	Response::methodNotAllowed() const {
	std::vector<std::string>	vctMethods;

	if (this->_locBlocSelect and this->_locBloc.getHttpMethodsSet())
		vctMethods = this->_locBloc.getHttpMethods();
	else
		vctMethods = this->_serv.getHttpMethods();

	for (size_t i = 0; i < vctMethods.size(); i++)
	{
		if (vctMethods[i] == this->_req.getMethod())
			return false;
	}
	return true;
}

void	Response::findRightCodeError() {
	if (this->methodNotAllowed())
		this->_statusCode = 405;
	else if (this->_isDir)
		this->_statusCode = 403;
	else
		this->_statusCode = 404;
}

std::string	Response::findRightError() {
	bool		pageFind = false;
	std::string	path;

	this->findRightCodeError();
	path = rightPathErr(pageFind, this->_statusCode, this->_serv, this->_locBlocSelect, this->_locBloc);

	std::ifstream tmp(path.c_str(), std::ios::in | std::ios::binary);

	if (this->_autoindex and !this->methodNotAllowed())
	{
		path = this->_defaultPage.createAutoindexPage(this->_path);
		this->_statusCode = 200;
	}
	else if (!tmp or !pageFind)
		path = this->_defaultPage.createDefaultPage(this->_statusCode);
	if (tmp)
		tmp.close();
	return path;
}

void	Response::httpRedir() {
	std::string	res;

	res = "HTTP/1.1 301 Moved Permanently\nLocation: ";
	if (this->_locBlocSelect and this->_locBloc.getHttpRedirSet())
		res += this->_locBloc.getHttpRedir();
	else
		res += this->_serv.getHttpRedir();
	res += "\n\n";
	fdEpollout(this->_req.getEpollFd(), this->_req.getFd());
	write(this->_req.getFd(), res.c_str(), res.size());
	fdEpollin(this->_req.getEpollFd(), this->_req.getFd());
	this->_closeConnection = true;
	return ;
}

std::string	Response::deleteResource() {
	struct stat	stat_buf;
	std::string	path;
	int			result;

	if (this->_path.size() > 0)
		path = this->_path[0];
	else
	{
		this->_statusCode = 204;
		return findRightPageError(this->_statusCode, this->_serv, this->_locBlocSelect, this->_locBloc);
	}
 
	result = stat(path.c_str(), &stat_buf);
	if (result != 0)
		this->_statusCode = 204;
	if (S_ISDIR(stat_buf.st_mode))
	{
		result = std::system(("rm -rf " + path).c_str());
		if (result == 0)
			this->_statusCode = 200;
		else
			this->_statusCode = 204;
	}
	else if (S_ISREG(stat_buf.st_mode))
	{
		result = std::remove(path.c_str());
		if (result == 0)
			this->_statusCode = 200;
		else
			this->_statusCode = 204;
	}
	else
	{
		this->_statusCode = 204;
	}

	return findRightPageError(this->_statusCode, this->_serv, this->_locBlocSelect, this->_locBloc);
}

void	Response::sendData() {
	std::string	path;
	bool		err;

	if (!(err = this->rightPath()))
		path = this->testAllPaths(&err);
	if ((err or this->methodNotAllowed()) && this->_req.getMethod() != "DELETE")
		path = findRightError();


	if (((this->_locBlocSelect and this->_locBloc.getHttpRedirSet()) or
		this->_serv.getHttpRedirSet()) and this->_statusCode != 405)
		return this->httpRedir();

	if (this->_req.getMethod() == "DELETE")
		path = this->deleteResource();
	this->sendHeader(path);
	this->printResponse();
}

std::string	Response::sendContentTypeError() {
	std::string	res;
	std::string	path;

	path = findRightPageError(this->_statusCode, this->_serv, this->_locBlocSelect, this->_locBloc);

	Header	header(path, &this->_statusCode);

	res = header.getHeaderRequestError();
	fdEpollout(this->_req.getEpollFd(), this->_req.getFd());
	write(this->_req.getFd(), res.c_str(), res.size());
	fdEpollin(this->_req.getEpollFd(), this->_req.getFd());
	return path;
}

void	Response::printResponse() const
{
	time_t		t;
	std::string	_time;

	std::time(&t);
	_time = std::ctime(&t);
	std::cout << "\033[1;34m[" << _time.substr(0, _time.length() - 1) << "]\033[0m";
	std::cout << "\033[1;36m [RESPONSE] \033[0m";
	std::cout << "\033[1;97m[" << this->_req.getHost() << ":" << this->_req.getPort() << "]\033[0m";
	std::cout << "\033[1;97m [" << this->_req.getMethod() << "\033[0m";
	std::cout << "\033[1;97m " << this->_req.getPath() << "]\033[0m";

	if (this->_statusCode == 200)
		std::cout << " - \033[1;32m" << this->_statusCode << "\033[0m";
	else
		std::cout << " - \033[1;31m" << this->_statusCode << "\033[0m";

	std::cout << "\033[1;97m " << getHttpStatusCodeMessage(this->_statusCode) << "\033[1;97m" << std::endl;
}

void	Response::sendHeader(std::string path)
{
	std::string		res;
	std::string		body;
	Header			header(path, &this->_statusCode, this);

	if (header.getContentType() == "406")
	{
		this->_statusCode = 406;
		path = sendContentTypeError();
	}
	else
	{
		if (this->_req.getCgiExtension().length())
		{
			Cgi	cgi(this->_serv, this->_req, header, this->_envp);
			
			std::cout << "/// EXECUTE CGI SCRIPT ///" << std::endl;
			
			cgi.execute(path, this->_serv.getCgi()[this->_req.getCgiExtension()], body);
		}

		res = header.getHeader();

		fdEpollout(this->_req.getEpollFd(), this->_req.getFd());
		if (send(this->_req.getFd(), res.c_str(), res.size(), MSG_NOSIGNAL) <= 0)
		{
			this->_closeConnection = true;
			std::cout << "Err: send call failed" << std::endl;
		}
		fdEpollin(this->_req.getEpollFd(), this->_req.getFd());

		if (this->_req.getMethod() != "HEAD")
			this->sendPage(path, body);
	}
}


void		Response::sendPage(std::string path_file, const std::string &cgi_content)
{
	std::string	body;

	if (cgi_content.length())
		body =  cgi_content;
	else
		body = fileToStr(path_file);
	
	fdEpollout(this->_req.getEpollFd(), this->_req.getFd());
	if (send(this->_req.getFd(), body.c_str(), body.length(), MSG_NOSIGNAL) <= 0)
	{
		this->_closeConnection = true;
		std::cout << "Err: send call failed" << std::endl;
	}
	fdEpollin(this->_req.getEpollFd(), this->_req.getFd());

	if (this->_req.getConnection() == "close")
		this->_closeConnection = true;
}

std::ostream	&operator<<(std::ostream &out, const Response &res)
{
	(void)res;
	out << "///////////////		REPONSE		////////////////\n";
	
	return (out);
}
