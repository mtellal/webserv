#include "Response.hpp"
#include "Cgi.hpp"

Response::Response() :
_locBlocSelect(false), _isDir(false), _autoindex(false),
_closeConnection(false)	{}

Response::Response(const Request &req, const Server &s, char **envp) :
_serv(s), _req(req), _locBlocSelect(false),
_isDir(false), _autoindex(false), _closeConnection(false),
_envp(envp), _defaultPage(_req, _serv)	{}

Response::Response(Response const &src) { *this = src; }

Response::~Response() {}

Response	&Response::operator=(Response const &rhs) {
	if (this != &rhs)
	{
		this->_serv = rhs._serv;
		this->_req = rhs._req;
		this->_path = rhs._path;
		this->_errPath = rhs._errPath;
		this->_httpRep = rhs._httpRep;
		this->_statusCode = rhs._statusCode;
		this->_locBlocSelect = rhs._locBlocSelect;
		this->_locBloc = rhs._locBloc;
		this->_isDir = rhs._isDir;
		this->_autoindex = rhs._autoindex;
		this->_closeConnection = rhs._closeConnection;
		this->_envp = rhs._envp;
		this->_defaultPage = rhs._defaultPage;
	}
	return *this;
}

//	GETTER
Server		Response::getServ() const {
	return this->_serv;
}

Request		Response::getRequest() const {
	return this->_req;
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
	std::string					root = this->rightRoot();
	std::string					newPath;
	std::vector<std::string>	index = this->rightIndex();

	memset(&fileOrDir, 0, sizeof(fileOrDir));
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
		return true;
	return false;
}

/*	Pareil que au dessus mais si un aucun bloc de Location est selectionne */
bool	Response::rightPathServer() {
	struct stat					fileOrDir;
	//std::string					root = this->rightRoot();
	std::string					root = this->_serv.getRoot();
	std::string					newPath;
	std::vector<std::string>	index;

	memset(&fileOrDir, 0, sizeof(fileOrDir));
	if (root[0] == '/')
		root.erase(0, 1);
	newPath = this->_req.getPath();
	root += newPath;
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
			}
			else
			{
				rightPath = this->_path[i];
				*err = false;
				this->_statusCode = 200;
				break;
			}
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

/*	On regarde par rapport au code erreur si une page a ete set pour ce code erreur
	dans notre fichier de configuration en reconstituant le path */
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
	path = this->rightPathErr(pageFind);

	std::ifstream tmp(path.c_str(), std::ios::in | std::ios::binary);

	if (this->_autoindex and !this->methodNotAllowed())
		path = this->_defaultPage.createAutoindexPage(this->_path);
	else if (!tmp or !pageFind)
		path = this->_defaultPage.createDefaultErrorPage(this->_statusCode);
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
	write(this->_req.getFd(), res.c_str(), res.size());
	this->_closeConnection = true;
	return ;
}

void	Response::sendData() {
	std::string	path;
	bool		err;

	if (!(err = this->rightPath()))
		path = this->testAllPaths(&err);
	if (err or this->methodNotAllowed())
		path = findRightError();

	if (((this->_locBlocSelect and this->_locBloc.getHttpRedirSet()) or
			this->_serv.getHttpRedirSet()) and this->_statusCode != 405)
		return this->httpRedir();

	this->sendHeader(path);
}

void	Response::sendContentTypeError() {
	std::string	res;
	std::string	path;

	path = this->_defaultPage.createDefaultErrorPage(this->_statusCode);

	Header	header(path, &this->_statusCode, this);

	res = header.getHeader();
	write(this->_req.getFd(), res.c_str(), res.size());
}

/*
	Creer un header avec une map => pouvoir acceder aux champs facilement avec offset op
*/

void	Response::sendHeader(std::string path)
{
	std::string		res;
	std::string		body;
	Header			header(path, &this->_statusCode, this);
	Cgi				cgi(this->_serv, this->_req, header, this->_envp);


	if (header.getContentType() == "406")
		sendContentTypeError();
	else
	{

		if (cgi.isCgiRequest())
		{

			std::cout << "cgi found" << std::endl;

			int	status = cgi.execute(path, body);

			(void)status;

			std::cout << "\n/////////////		BODY	///////////\n" << body << std::endl;

			header = cgi.getHeader();

			std::cout << "\n//////////	HEADER	///////////\n" << header.getHeader() << std::endl;
		}
		else
		{
			std::cout << "not an cgi request" << std::endl;
		}

		res = header.getHeader();
		write(this->_req.getFd(), res.c_str(), res.size());
		this->sendPage(path, body);
	}
}

std::string	Response::contentFile(const std::string &path_file)
{
	std::ifstream	file(path_file.c_str(), std::ios::in | std::ios::binary);
	std::string		page;

	if (file)
	{
		std::ostringstream ss;
		ss << file.rdbuf();
		page = ss.str();
	}
	file.close();
	return (page);
}

void		Response::sendPage(std::string path_file, const std::string &cgi_content)
{
	std::string	content;

	if (cgi_content.length())
		content =  cgi_content;
	else
		content = contentFile(path_file);
	
	if (send(this->_req.getFd(), content.c_str(), content.length(), 0) == -1)
		perror("send call failed");

	//std::cout << content << std::endl;
	//write(this->_req.getFd(), content.c_str(), content.size());

	if (this->_req.getConnection() == "close")
		this->_closeConnection = true;
}

void	Response::selectLocationBlock() {
	std::vector<Location>	locations = this->_serv.getVctLocation();
	std::string 			strBlocLoc;
	Location				tmp;
	std::string				req = this->_req.getPath();
	size_t j;

	for (size_t i = 0; i < locations.size(); i++)
	{
		strBlocLoc = locations[i].getPath();
		j = 0;
		if (!strncmp(strBlocLoc.c_str(), this->_req.getPath().c_str(), strBlocLoc.length()))
		{
			j += strBlocLoc.length();
			if (j && strBlocLoc.size() > tmp.getPath().size())
			{
				this->_locBlocSelect = true;
				tmp = locations[i];
			}
		}
	}
	if (!this->_locBlocSelect)
	{
		for (size_t i = 0; i < locations.size(); i++)
		{
			if (locations[i].getPath() == "/")
			{
				this->_locBlocSelect = true;
				tmp = locations[i];
				break ;
			}
		}
	}
	if (this->_locBlocSelect)
		this->_locBloc = tmp;
}

std::ostream	&operator<<(std::ostream &out, const Response &res)
{
	(void)res;
	out << "///////////////		REPONSE		////////////////\n";
	
	return (out);
}
