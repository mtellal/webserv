#include "Request.hpp"
#include "DefaultPage.hpp"
#include "Header.hpp"
#include "utils.hpp"
#include <string.h>
#include <sys/epoll.h>
#include <fstream>

Request::Request() {}

Request::Request(int fd, const std::vector<Server> &servers, std::map<int, int> clientServerFds, int epollFd) : 
_hostSet(false), _tooLarge(false), _agentSet(false), _acceptSet(false), 
_methodSet(false), _errRequest(false), _refererSet(false), _badRequest(false),
_boundarySet(false), _awaitingBody(false), _connectionSet(false), _queryStringSet(false), 
_bodyFileExists(false), _awaitingHeader(false), _closeConnection(false), _locBlocSelect(false),
_fd(fd), _epollFd(epollFd), _firstReqBytesRecieved(0), _serverName("Webserv/1.0"), _bodyFilePath(".bodyfile"), _servers(servers),
_clientServerFds(clientServerFds)
{
	this->functPtr[0] = &Request::setHostPort;
	this->functPtr[1] = &Request::setConnection;
	this->functPtr[2] = &Request::setAccept;
	this->functPtr[3] = &Request::setReferer;
	this->functPtr[4] = &Request::setAgent;
	this->functPtr[5] = &Request::setAuthentification;
	this->functPtr[6] = &Request::setContentLength;
	this->functPtr[7] = &Request::setContentType;
}

Request::Request(Request const &src) {
	*this = src;
}

Request::~Request() {}

Request	&Request::operator=(Request const &rhs) {
	if (this != &rhs)
	{

		this->_hostSet = rhs._hostSet;
		this->_tooLarge = rhs._tooLarge;
		this->_agentSet = rhs._agentSet;
		this->_acceptSet = rhs._acceptSet;
		this->_methodSet = rhs._methodSet;

		this->_errRequest = rhs._errRequest;
		this->_refererSet = rhs._refererSet;
		this->_badRequest = rhs._badRequest;
		this->_boundarySet = rhs._boundarySet;
		this->_awaitingBody = rhs._awaitingBody;
		this->_connectionSet = rhs._connectionSet;
		this->_queryStringSet = rhs._queryStringSet;
		this->_bodyFileExists = rhs._bodyFileExists;
		this->_awaitingHeader = rhs._awaitingHeader;
		this->_closeConnection = rhs._closeConnection;
		this->_locBlocSelect = rhs._locBlocSelect;

		this->_fd = rhs._fd;
		this->_epollFd = rhs._epollFd;
		this->_bodyBytesRecieved = rhs._bodyBytesRecieved;
		this->_firstReqBytesRecieved = rhs._firstReqBytesRecieved;

		this->_host = rhs._host;
		this->_path = rhs._path;
		this->_port = rhs._port;
		this->_agent = rhs._agent;
		this->_method = rhs._method;
		this->_accept = rhs._accept;
		this->_referer = rhs._referer;
		this->_request = rhs._request;
		this->_boundary = rhs._boundary;
		this->_connection = rhs._connection;
		this->_serverName = rhs._serverName;
		this->_httpVersion = rhs._httpVersion;
		this->_queryString = rhs._queryString;
		this->_contentType = rhs._contentType;
		this->_bodyFilePath = rhs._bodyFilePath;
		this->_cgiExtension = rhs._cgiExtension;
		this->_contentLength = rhs._contentLength;
		this->_authentification = rhs._authentification;

		this->_servers = rhs._servers;
		this->_servBlock = rhs._servBlock;
		this->_locationBlock = rhs._locationBlock;

	}
	return *this;
}

void		Request::setErrorRequest(bool v)
{
	this->_errRequest = v;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//												G E T T E R													  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool								Request::getAgentSet() const { return this->_agentSet; }
bool								Request::getAcceptSet() const { return this->_acceptSet; }
bool								Request::getErrRequest() const { return this->_errRequest; }
bool								Request::getRefererSet() const { return this->_refererSet; }
bool								Request::getBadRequest() const { return this->_badRequest; }
bool								Request::getConnectionSet() const { return this->_connectionSet; }
bool								Request::getBodyFileExists() const { return this->_bodyFileExists; }
bool								Request::getcloseConnection() const { return this->_closeConnection; }
bool								Request::getAwaitingRequest() const { return (this->_awaitingHeader || this->_awaitingBody); }
bool								Request::getLocBlocSelect() const { return this->_locBlocSelect; }

int									Request::getFd() const { return this->_fd; }
int									Request::getEpollFd() const { return this->_epollFd; }

size_t								Request::getBytesRecievd() const { return (this->_bodyBytesRecieved); }

std::string							Request::getPath() const { return this->_path;}
std::string							Request::getHost() const { return this->_host; }
std::string							Request::getPort() const { return this->_port; }
std::string							Request::getAgent() const { return this->_agent; }
std::string							Request::getMethod() const { return this->_method; }
std::string							Request::getAccept() const { return this->_accept; }
std::string							Request::getReferer() const { return this->_referer; }
std::string							Request::getConnection() const { return this->_connection; }
std::string							Request::getServerName() const { return this->_serverName; }
std::string							Request::getContentType() const { return this->_contentType; }
std::string							Request::getHttpVersion() const { return this->_httpVersion; }
std::string							Request::getQueryString() const { return this->_queryString; }
std::string							Request::getBodyFilePath() const { return (this->_bodyFilePath); }
std::string							Request::getContentLength() const { return this->_contentLength; }
std::string							Request::getAuthentification() const { return this->_authentification; }
std::string							Request::getCgiExtension() const { return (this->_cgiExtension); }

Server								Request::getServBlock() const { return (this->_servBlock); }
Location							Request::getLocationBlock() const { return (this->_locationBlock); }

void								Request::getErrorPage(const std::string &errMsg, int statusCode) {
	std::string	path;
	std::string	strHeader;
	std::string	page;

	if (this->_tooLarge)
		statusCode = 413;
	if (!this->_methodSet || !this->_hostSet || this->_badRequest)
		statusCode = 400;

	if (errMsg.length())
		this->errMessage(statusCode, errMsg);

	path = findRightPageError(statusCode, this->_servBlock, this->_locBlocSelect, this->_locationBlock);

	Header header(path, &statusCode);
	page = header.getHeaderRequestError();

	page += fileToStr(path);
	if (send(this->_fd, page.c_str(), page.size(), MSG_NOSIGNAL) <= 0)
		perror("send call failed");
	this->_closeConnection = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//												S E T T E R													  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void							Request::addQueryString(const std::string &key, const std::string &value)
{
	if (this->_queryString.length())
		this->_queryString += "&" + key + "=" + value;
	else
		this->_queryString +=  key + "=" + value;
}

void							Request::parsArgs(std::string arg) {
	std::vector<std::string>	args;
	std::vector<std::string>	keyValue;

	this->_queryStringSet = true;
	args = ft_split(arg.c_str(), "&");
	for (size_t i = 0; i < args.size(); i++)
	{
		keyValue = ft_split(args[i].c_str(), "=");
		if (keyValue.size() == 1)
			this->addQueryString(keyValue[0], "");
		else
			this->addQueryString(keyValue[0], keyValue[1]);
	}
}

bool							Request::setMethodVersionPath(const std::string &buff)
{
	std::vector<std::string>	strSplit;
	std::vector<std::string>	lines;
	std::string					methods[4] = { "GET", "POST", "HEAD", "DELETE" };

	lines = ft_split(buff, "\r\n");
	if (!lines.size())
		return (false);
	strSplit = ft_split(lines[0], " ");
	if (strSplit.size() != 3)
		return (false);
	else
	{
		for (size_t i = 0; i < 4; i++)
		{
			if (methods[i] == strSplit[0])
				this->_method = strSplit[0];
		}
		if (!this->_method.length())
			return (false);
		if (strSplit[1][0] != '/' || (strSplit[2] != "HTTP/1.0" && strSplit[2] != "HTTP/1.1"))
			return (false);
		this->_httpVersion = strSplit[2];
		strSplit = ft_split(strSplit[1].c_str(), "?");
		if (strSplit.size() == 2)
			this->parsArgs(strSplit[1]);
		this->_path = strSplit[0];
		this->_methodSet = true;
	}
	return (true);
}

bool							checkUri(const std::string &host)
{
	for (size_t i = 0; i < host.length(); i++)
		if (host[i] == '/')
			return (false);
	return (true);
}

void							Request::setHostPort(std::vector<std::string> strSplit)
{
	if (this->_hostSet
			|| strSplit.size() < 2
			|| (strSplit.size() >= 2 && !checkUri(strSplit[1])))
	{
		this->_hostSet = false;
		return ;
	}
	if (strSplit.size() >= 2)
		this->_host = strSplit[1];
	if (strSplit.size() >= 3)
		this->_port = strSplit[2];
	this->_hostSet = true;
}

void							Request::setConnection(std::vector<std::string> strSplit) {
	this->_connection = strSplit[1];
	this->_connectionSet = true;
}

void							Request::setAccept(std::vector<std::string> strSplit) {
	this->_accept = strSplit[1];
	this->_acceptSet = true;
}

void							Request::setReferer(std::vector<std::string> strSplit) {
	this->_referer = strSplit[1];
	this->_refererSet = true;
}

void							Request::setAgent(std::vector<std::string> strSplit) {
	for (size_t i = 1; i < strSplit.size(); i++)
	{
		if (i != 1)
			this->_agent += " ";
		this->_agent += strSplit[i];
	}
	this->_agentSet = true;
}

void							Request::setAuthentification(std::vector<std::string> strSplit) {
	for (size_t i = 1; i < strSplit.size(); i++)
	{
		if (i != 1)
			this->_authentification += " ";
		this->_authentification += strSplit[i];
	}
}

void							Request::setContentLength(std::vector<std::string> strSplit) {
	this->_contentLength = strSplit[1];
}

void							Request::setContentType(std::vector<std::string> strSplit) {

	this->_contentType = strSplit[1];
	strSplit = ft_split(strSplit[1], " ");
	if (strSplit.size() == 2 and strSplit[0] == "multipart/form-data;" and
		strSplit[1].find("boundary=") != std::string::npos)
	{
		strSplit = ft_split(strSplit[1].c_str(), "=\"");
		if (strSplit.size() < 2)
			return ;
		this->_boundarySet = true;
		this->_boundary = "--";
		this->_boundary += strSplit[1];
	}

}

void							Request::setGetParams(std::vector<std::string> vct, size_t *i) {
	std::vector<std::string>	strSplit;
	std::string					endBoundary = this->_boundary + "--";
	std::string					name;
	std::string					value;

	while (*i < vct.size() and vct[*i] != endBoundary)
	{
		strSplit = ft_split(vct[*i].c_str(), " ");
		if (strSplit[0] == "Content-Disposition:")
		{
			for (size_t j = 0; j < strSplit.size(); j++)
			{
				if (strSplit[j].find("name=") != std::string::npos)
				{
					name = ft_split(strSplit[j].c_str(), "=\"")[1];
					break ;
				}
			}
			*i += 2;
			value = vct[*i];
			this->addQueryString(name, value);
		}
		*i += 1;
	}
}

void	Request::setBytesRecieved(size_t bytes) { this->_bodyBytesRecieved = bytes; }

int		Request::setServBlock()
{
	int	idxServBlock;

	if ((idxServBlock = pickServBlock()) == -1)
		return (-1);
	this->_servBlock = this->_servers[idxServBlock];
	this->_servBlock.setSocket(idxServBlock);
	this->selectLocationBlock();

	return (0);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										M E M B E R S   F U N C T I O N S 									  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int		Request::selectBlockWithServerName(std::vector<Server> vctServSelect, std::vector<int> index) {
	std::vector<std::string>	serverName;

	for (size_t i = 0; i < vctServSelect.size(); i++)
	{
		if (vctServSelect[i].getServerNameSet())
		{
			serverName = vctServSelect[i].getServerName();
			for (size_t j = 0; j < serverName.size(); j++)
			{
				if (serverName[j] == this->_host)
					return index[i];
			}
		}
	}
	return index[0];
}

//	verif si aucun host peut etre envoyer, ex: 'Host: ""' 
int		Request::pickServBlock()
{
	std::vector<Server>	vctServSelect;
	std::vector<int>	index;
	std::string			host;
	std::string			port;

	host = this->_servers[this->_clientServerFds[this->_fd]].getHost();
	port = this->_servers[this->_clientServerFds[this->_fd]].getPort();
	if (host != "0.0.0.0")
	{
		for (size_t i = 0; i < this->_servers.size(); i++)
		{
			if (this->_servers[i].getHostSet() &&
				(host == getRightHost(this->_servers[i].getHost()) ||
				host == this->_servers[i].getHost()) &&
				port == this->_servers[i].getPort())
			{
				vctServSelect.push_back(this->_servers[i]);
				index.push_back(i);
			}
		}
		if (vctServSelect.size() == 1)
			return index[0];
		else if (vctServSelect.size() > 1)
			return selectBlockWithServerName(vctServSelect, index);
	}
	for (size_t i = 0; i < this->_servers.size(); i++)
	{
		if (!this->_servers[i].getHostSet() &&
			(port == this->_servers[i].getPort()))
		{
			if (ft_split(host.c_str(), ".").size() == 4)
			{
				vctServSelect.push_back(this->_servers[i]);
				index.push_back(i);
			}
			else if (this->_servers[i].getServerNameSet())
			{
				for (size_t j = 0; j < this->_servers[i].getServerName().size(); j++)
				{
					if (this->_host == this->_servers[i].getServerName()[j])
					{
						vctServSelect.push_back(this->_servers[i]);
						index.push_back(i);
					}
				}
			}
		}
	}
	if (vctServSelect.size() == 1)
		return index[0];
	else if (vctServSelect.size() > 1)
		return selectBlockWithServerName(vctServSelect, index);
	return -1;
}

void							Request::quitAwaitingRequest()
{
	remove(this->_bodyFilePath.c_str());
	this->_bodyFileExists = false;
	this->_awaitingBody = false;
	this->_awaitingHeader = false;
}

void	Request::selectLocationBlock() {
	std::vector<Location>	locations = this->_servBlock.getVctLocation();
	std::string 			strBlocLoc;
	Location				tmp;
	std::string				req = this->getPath();
	size_t j;

	for (size_t i = 0; i < locations.size(); i++)
	{
		strBlocLoc = locations[i].getPath();
		j = 0;
		if (!strncmp(strBlocLoc.c_str(), this->getPath().c_str(), strBlocLoc.length()))
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
		this->_locationBlock = tmp;
}

void						Request::extractContentDisposition(const std::string &line, std::string &filename)
{
	std::string 				tmp(line);
	std::vector<std::string>	v;
	std::vector<std::string>	keys;
	std::vector<std::string>	tmp2;

	v = ft_split(tmp, ";");
	for (size_t i = 1; i < v.size(); i++)
	{
		trimSpaceFront(v[i]);
		if (!memcmp(v[i].c_str(), "filename=", 9))
		{
			tmp2 = ft_split(v[i], "=");
			if (tmp2.size() == 2)
			{
				filename = tmp2[1];
				filename = removeChar(filename, '"');
				trimSpaceBack(filename);
			}
		}
	}
}

/*
	Verify if the data inside boundaries need to be save via the contentType, 
	then parse and save it with the proper file name
*/

void							Request::extractFile(const std::string &bound_data)
{
	size_t						index;
	std::string					fileName;
	std::string					rootPath;
	std::string					uploadPath;
	std::string					contentType;
	std::ofstream				outfile;
	std::vector<std::string>	fields;

	index = bound_data.find("\r\n\r\n");
	if (index == (size_t)-1)
		return ;

	fields = ft_split(bound_data.substr(0, index), "\r\n");
	for (size_t i = 0; i < fields.size(); i++)
	{
		if (!memcmp(fields[i].c_str(), "Content-Disposition:", 20))
			this->extractContentDisposition(fields[i], fileName);
	}

	if (fileName.length())
	{
		uploadPath = this->_servBlock.getUpload();
		rootPath = this->_servBlock.getRoot();

		if (!uploadPath.length())
			return (this->getErrorPage("Upload path not defined"));
		
		formatPath(uploadPath);
		formatPath(rootPath);

		if (memcmp(uploadPath.c_str(), rootPath.c_str(), rootPath.length()))
			return (this->getErrorPage("Upload path not in " + this->_servBlock.getRoot() + " folder", 403));

		fileName = uploadPath + fileName;

		outfile.open(fileName.c_str(),
			std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);

		if (!outfile.is_open())
		{
			this->getErrorPage("can't open file \"" + fileName + "\"");
			return ;
		}

		outfile.write(bound_data.c_str() + index + 4, bound_data.length() - index - 4 - 2);
		outfile.close();
	}
}

/*
	Extract and save the datas/files between boundarys
*/

void							Request::verifyFiles()
{
	std::string					request;
	std::vector<std::string>	bounds;

	request = fileToStr(this->_bodyFilePath);
	if (!request.length())
		return ;
	if (!memcmp("multipart/form-data", this->_contentType.c_str(), 19)
			&& this->_boundarySet)
	{
		bounds = ft_split_str(request, this->_boundary);
		for (size_t i = 0; i < bounds.size() && bounds[i] != "--\r\n"; i++)
			this->extractFile(bounds[i]);
	}
}

bool							validUploadPath(const std::string &path)
{
	struct stat f;
	std::string	p(path);

	formatPath(p);
	memset(&f, 0, sizeof(f));
	if (stat(p.c_str(), &f) == -1)
		return (false);
	if (S_ISDIR(f.st_mode))
		return (true);
	return (false);
}

void							Request::checkBodyBytesRecieved()
{
	if (ft_stoi(this->_contentLength, NULL) != (int)this->_bodyBytesRecieved)
		this->_awaitingBody = true;
	else
	{
		if (!this->_cgiExtension.length())
		{
			if (validUploadPath(this->_servBlock.getUpload()))
			{
				this->verifyFiles();
				remove(this->_bodyFilePath.c_str());
			}
			else
				this->errInfoMessage("invalid path upload");
		}
		this->_awaitingBody = false;
	}
}

int							Request::recvToBodyFile(int fd, std::ofstream &out)
{
	char	buff[BUFFLEN_FILE + 1];
	int		bytes;

	bytes = recv(fd, buff, BUFFLEN_FILE, 0);
	
	if (bytes < 1)
	{
		out.close();
		if (bytes == -1)
			this->getErrorPage("recv failed");
		if (!bytes)
			this->_closeConnection = true;
		this->quitAwaitingRequest();
		return (-1);
	}

	buff[bytes] = '\0';
	out.write(buff, bytes);
	out.close();
	this->_bodyBytesRecieved += bytes;
	return (0);
}

int								Request::openBodyFile(std::ofstream &out)
{
	std::string		errMsg;

	if (this->_bodyFileExists)
		out.open(this->_bodyFilePath.c_str(),
			std::ofstream::out | std::ofstream::ate | std::ofstream::app | std::ofstream::binary);
	else
	{
		out.open(this->_bodyFilePath.c_str(), std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
		this->_bodyFileExists = true;
	}

	if (!out.is_open())
	{
		this->quitAwaitingRequest();
		this->getErrorPage("can't open file \"" + this->_bodyFilePath + "\"");
		return (-1);
	}

	return (0);
}

/*
	Save request data in bodyFile,
	and verify if it is the last request
*/

void							Request::awaitingBody(int fd)
{
	std::ofstream 				out;

	if (openBodyFile(out) == -1)
		return ;

	if (recvToBodyFile(fd, out) == -1)
		return ;

	this->checkBodyBytesRecieved();

	return ;
}

void							Request::checkCgiPath()
{
	const char										*point;
	std::map<std::string, std::string>::iterator	it;	
	std::map<std::string, std::string>				mapCgi;

	mapCgi = this->_servBlock.getCgi();
	if (!mapCgi.size())
		return ;
	point = strrchr(this->_path.c_str(), '.');
	if (!point)
		return ;
	it = mapCgi.begin();
	while (it != mapCgi.end())
	{
		if (!memcmp(point + 1, it->first.c_str(), it->first.length())
				&& (*(point + 1 + it->first.length()) == '\0' || *(point + 1 + it->first.length()) == '?'))
			{
				this->_cgiExtension = it->first;
				return;
			}
		it++;
	}
}

/*
	Save first part body of a POST request 
*/

void	Request::bodyRequest(size_t index)
{
	std::ofstream 	out;

	if (openBodyFile(out) == -1)
		return ;

	out.write(this->_request.c_str() + index + 4,  this->_bodyBytesRecieved);

	out.close();
}

void						Request::setHTTPFields(const std::string &header)
{
	std::vector<std::string>	vct;
	std::vector<std::string>	strSplit;
	std::string					key[8] = { "host:",
				"connection:", "accept:", "referer:", "user-agent:", "authentification:",
				"content-length:", "content-type:"};
	std::string					field;
	size_t						index;

	vct = ft_split(header, "\n\r");
	for (size_t i = 0; i < vct.size(); i++)
	{
		index = vct[i].find(':');
		if (index != (size_t)-1)
		{
			field = vct[i].substr(0, index + 1);
			lowerCaseStr(field);
			for (size_t j = 0; j < 8; j++)
			{
				if (field.length() && !memcmp(field.c_str(), key[j].c_str(), key[j].length()))
				{
					strSplit = ft_split(vct[i], ":");
					if (strSplit.size() < 2)
						break;
					trimSpace(strSplit[1]);
					(this->*functPtr[j])(strSplit);
					break ;
				}
			}
		}
	}
}

/*
	determiner la requete => err direct tant que n
*/

int						Request::awaitingHeader(int fd)
{
	char		buff[BUFFLEN + 1];
	int			bytes;
	size_t		index;


	bytes = recv(fd, buff, BUFFLEN, 0);

	if (bytes < 1)
	{
		if (!bytes)
			this->_closeConnection = true;
		else if (bytes == -1)
			this->getErrorPage("recv call failed");
		return (-1);
	}

	buff[bytes] = '\0';

	if (!this->_methodSet && bytes == 2 && !memcmp(buff, "\r\n", 2))
	{
		this->_awaitingHeader = true;
		return (-1);
	}

	if (!this->_methodSet && !setMethodVersionPath(buff))
	{
		this->getErrorPage("Invalid HTTP request", 400);
		this->_awaitingHeader = false;
		return (-1);
	}

	this->_firstReqBytesRecieved += bytes;
	this->_request.append(buff, bytes);

	if (this->_methodSet && (index = this->_request.find("\r\n\r\n")) != (size_t)-1)
	{
		this->_awaitingHeader = false;
		return (this->_firstReqBytesRecieved);
	}

	this->_awaitingHeader = true;
	return (-1);
}

void						Request::request(int fd)
{
	int				bytesRecievd;
	size_t			index;
	std::string		header;
	std::string		body;

	bytesRecievd = 0;
	if (this->_awaitingBody)
	{
		this->awaitingBody(fd);
	}
	else if ((bytesRecievd = this->awaitingHeader(fd)) != -1)
	{
		index = this->_request.find("\r\n\r\n");
		header = this->_request.substr(0, index);

		this->setHTTPFields(header);

		printRequest();

		if (this->setServBlock() == -1)
			return (this->getErrorPage("Host unavailable"));
		
		checkCgiPath();

		if (this->_methodSet && this->_method == "POST")
		{
			if (!this->_contentLength.length())
				return (this->getErrorPage("Bad Request", 411));
			if (bytesRecievd > (int)index + 4)
				body = this->_request.substr(index + 4, bytesRecievd);
			
			this->_bodyBytesRecieved = bytesRecievd - (index + 4);

			if (body.length())
				this->bodyRequest(index);

			this->checkBodyBytesRecieved();
		}

		if (this->_contentLength.length()
				&& (ft_stoi(this->_contentLength, NULL) > MAX_BYTES_UPLOAD
					|| ((this->_servBlock.getClientMaxBodySize()
						&& ft_stoi(this->_contentLength, NULL) > (this->_servBlock.getClientMaxBodySize() * 1000000)))))
			this->getErrorPage("Request Entity Too Large", 413);

		if (!this->_hostSet || this->_badRequest)
			this->getErrorPage("Bad Request", 400);
	}
}

void	Request::printRequest() const
{
	time_t		t;
	std::string	_time;

	std::time(&t);
	_time = std::ctime(&t);
	std::cout << "\033[1;34m[" << _time.substr(0, _time.length() - 1) << "]\033[0m";
	std::cout << "\033[1;36m [REQUEST] \033[0m";
	std::cout << "\033[1;97m[" << this->_host << ":" << this->_port << "]\033[0m";
	std::cout << "\033[1;97m " << this->_method << "\033[0m";
	std::cout << "\033[1;97m " << this->_path << "\033[0m";
	std::cout << "\033[1;90m - " << this->_agent << "\033[0m" << std::endl;
}

void	Request::errInfoMessage(const std::string &errMsg)
{
	std::cerr << "\033[1;97m[REQUEST]\033[0m \033[1;31merror:\033[0m \033[1;97m" << errMsg << "\033[0m" << std::endl;
}

void	Request::errMessage(int statusCode, const std::string &errMsg)
{
	time_t 		t;
	std::string	_time;

	std::time(&t);
	_time = std::ctime(&t);
	errInfoMessage(errMsg);
	std::cerr << "\033[1;34m[" << _time.substr(0, _time.length() - 1) << "]\033[0m";
	std::cerr << "\033[1;36m [RESPONSE] \033[0m";

	if (this->_hostSet)
		std::cerr << "\033[1;97m[" << this->_host << ":" << this->_port << "]\033[0m";

	if (this->_methodSet)
	{
		std::cerr << "\033[1;97m [" << this->_method << "\033[0m";
		std::cerr << "\033[1;97m " << this->_path << "]\033[0m";
	}

	if (statusCode == 200)
		std::cerr << " - \033[1;32m" << statusCode << "\033[0m";
	else
		std::cerr << " - \033[1;31m" << statusCode << "\033[0m";

	std::cerr << "\033[1;97m " << getHttpStatusCodeMessage(statusCode) << "\033[1;97m" << std::endl;
}

std::ostream &operator<<(std::ostream & o, Request const & rhs) {
	o << "method = " << rhs.getMethod() << std::endl;
	o << "port = " << rhs.getPort() << std::endl;
	o << "host = " << rhs.getHost() << std::endl;
	o << "serv = " << rhs.getServBlock().getFd() << std::endl;
	o << "path = " << rhs.getPath() << std::endl;
	o << "contentLength = " << rhs.getContentLength() << std::endl;
	o << "cgiExtension = " << rhs.getCgiExtension() << std::endl;
	o << "httpVersion = " << rhs.getHttpVersion() << std::endl;
	return o;
}
