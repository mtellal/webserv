#include "Request.hpp"
#include "DefaultPage.hpp"
#include "Header.hpp"
#include "utils.hpp"
#include <string.h>
#include <sys/epoll.h>
#include <fstream>

Request::Request() {}

Request::Request(int fd) : 
_fd(fd), _errRequest(false), _queryStringSet(false), _boundarySet(false),
_closeConnection(false), _connectionSet(false), _acceptSet(false),
_refererSet(false), _agentSet(false), _serverName("Webserv/1.0"),
_methodSet(false), _hostSet(false), _tooLarge(false),
_badRequest(false),
_bodyBytesRecieved(0), _bodyFileExists(false), _bodyFilePath("./uploads/bodyfile"),
_awaitingHeader(false), _awaitingBody(false)
{
	this->functPtr[0] = &Request::setMethodVersionPath;
	this->functPtr[1] = &Request::setMethodVersionPath;
	this->functPtr[2] = &Request::setMethodVersionPath;
	this->functPtr[3] = &Request::setMethodVersionPath;
	this->functPtr[4] = &Request::setHostPort;
	this->functPtr[5] = &Request::setConnection;
	this->functPtr[6] = &Request::setAccept;
	this->functPtr[7] = &Request::setReferer;
	this->functPtr[8] = &Request::setAgent;
	this->functPtr[9] = &Request::setAuthentification;
	this->functPtr[10] = &Request::setContentLength;
	this->functPtr[11] = &Request::setContentType;
}

Request::Request(Request const &src) {
	*this = src;
}

Request::~Request() {}

Request	&Request::operator=(Request const &rhs) {
	if (this != &rhs)
	{
		this->_fd = rhs._fd;
		this->_errRequest = rhs._errRequest;
		this->_queryStringSet = rhs._queryStringSet;
		this->_boundarySet = rhs._boundarySet;
		this->_closeConnection = rhs._closeConnection;
		this->_boundary = rhs._boundary;

		this->_method = rhs._method;
		this->_path = rhs._path;
		this->_httpVersion = rhs._httpVersion;
		this->_host = rhs._host;
		this->_port = rhs._port;

		this->_connection = rhs._connection;
		this->_connectionSet = rhs._connectionSet;
		this->_accept = rhs._accept;
		this->_acceptSet = rhs._acceptSet;
		this->_queryString = rhs._queryString;
		this->_refererSet = rhs._refererSet;
		this->_referer = rhs._referer;
		this->_agentSet = rhs._agentSet;
		this->_agent = rhs._agent;

		this->_serverName = rhs._serverName;
		this->_authentification = rhs._authentification;
		this->_contentLength = rhs._contentLength;
		this->_contentType = rhs._contentType;
		this->_methodSet = rhs._methodSet;
		this->_hostSet = rhs._hostSet;
		this->_tooLarge = rhs._tooLarge;
		this->_badRequest = rhs._badRequest;

		this->_bodyBytesRecieved = rhs._bodyBytesRecieved;
		this->_bodyFileExists = rhs._bodyFileExists;
		this->_bodyFilePath = rhs._bodyFilePath;

		this->_request = rhs._request;
		this->_awaitingHeader = rhs._awaitingHeader;
		this->_awaitingBody = rhs._awaitingBody;
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

bool								Request::getErrRequest() const { return this->_errRequest; }

bool								Request::getcloseConnection() const { return this->_closeConnection; }

bool								Request::getConnectionSet() const { return this->_connectionSet; }

bool								Request::getAcceptSet() const { return this->_acceptSet; }

bool								Request::getRefererSet() const { return this->_refererSet; }

bool								Request::getAgentSet() const { return this->_agentSet; }

bool								Request::getBadRequest() const { return this->_badRequest; }

bool								Request::getAwaitingRequest() const { return (this->_awaitingHeader || this->_awaitingBody); }

bool								Request::getBodyFileExists() const { return (this->_bodyFileExists); }

int									Request::getFd() const { return this->_fd; }

size_t								Request::getBytesRecievd() const { return (this->_bodyBytesRecieved); }

std::string							Request::getMethod() const { return this->_method; }

std::string							Request::getPath() const { return this->_path;}

std::string							Request::getHttpVersion() const { return this->_httpVersion; }

std::string							Request::getHost() const { return this->_host; }

std::string							Request::getPort() const { return this->_port; }

std::string							Request::getConnection() const { return this->_connection; }

std::string							Request::getReferer() const { return this->_referer; }

std::string							Request::getAgent() const { return this->_agent; }

std::string							Request::getServerName() const { return this->_serverName; }

std::string							Request::getAuthentification() const { return this->_authentification;}

std::string							Request::getContentLength() const { return this->_contentLength; }

std::string							Request::getContentType() const { return this->_contentType; }

std::string							Request::getAccept() const { return this->_accept; }

std::string							Request::getQueryString() const { return this->_queryString; }

void								Request::getErrorPage() {
	std::string	path;
	std::string	strHeader;
	std::string	page;
	int			statusCode;

	if (this->_tooLarge)
		statusCode = 413;
	if (!this->_methodSet)
		statusCode = 400;
	else
		statusCode = 500;

	DefaultPage	defaultPage;
	path = defaultPage.createDefaultPage(statusCode);

	Header header(path, &statusCode);
	strHeader = header.getHeaderRequestError();
	send(this->_fd, strHeader.c_str(), strHeader.size(), MSG_NOSIGNAL);

	page = fileToStr(path);
	send(this->_fd, page.c_str(), page.size(), MSG_NOSIGNAL);
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

void							Request::setMethodVersionPath(std::vector<std::string> strSplit) {
	std::vector<std::string>	splitBis;

		if (strSplit.size() != 3)
			this->_badRequest = true;
		else
		{
			this->_method = strSplit[0];
			if (strSplit[2] != "HTTP/1.0" && strSplit[2] != "HTTP/1.1")
				this->_badRequest = true;
			this->_httpVersion = strSplit[2];
			strSplit = ft_split(strSplit[1].c_str(), "?");
			if (strSplit.size() == 2)
				this->parsArgs(strSplit[1]);
			this->_path = strSplit[0];
			this->_methodSet = true;
		}
}

void							Request::setHostPort(std::vector<std::string> strSplit) {
	strSplit = ft_split(strSplit[1].c_str(), ":");
	this->_host = strSplit[0];
	this->_port = strSplit[1];
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
	for (size_t i = 1; i < strSplit.size(); i++)
	{
		if (i != 1)
			this->_contentType += " ";
		this->_contentType += strSplit[i];
	}
	if (strSplit.size() == 3 and strSplit[1] == "multipart/form-data;" and
		strSplit[2].find("boundary=") != std::string::npos)
	{
		strSplit = ft_split(strSplit[2].c_str(), "=\"");
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
					// Tester si dans le code html, il n'y a pas de name ou que le name est vide
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

void							Request::setBytesRecieved(size_t bytes) { this->_bodyBytesRecieved = bytes; }


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										M E M B E R S   F U N C T I O N S 									  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void						Request::extractContentDisposition(const std::string &line, std::string &name, std::string &filename)
{
	std::string 				tmp(line);
	std::vector<std::string>	v;
	std::vector<std::string>	keys;

	v = ft_split(tmp, " ");

	for (size_t i = 0; i < v.size(); i++)
	{
		if (!memcmp(v[i].c_str(), "filename", 8))
		{
			filename = strtrim_char(v[i].substr(9, v[i].length()), ';');
			filename = strtrim_char(filename, '"');
		}
		else if (!memcmp(v[i].c_str(), "name", 4))
		{
			name = strtrim_char(v[i].substr(5, v[i].length()), ';');
			name = strtrim_char(name, '"');
		}
	}
}

void							Request::quitAwaitingRequest()
{
	remove(this->_bodyFilePath.c_str());
	this->_bodyFileExists = false;
	this->_awaitingBody = false;
	this->_awaitingHeader = false;
}

/*
	Verify if the data inside boundaries need to be save via the contentType, 
	then parse and save it with the proper file name
*/

void							Request::extractFile(const std::string &bound_data)
{
	size_t						index;
	std::string					contentType;
	std::string					fileName;
	std::string					name;
	std::ofstream				outfile;
	std::vector<std::string>	fields;


	index = bound_data.find("\r\n\r\n");

	if (index == (size_t)-1)
		return ;

	fields = ft_split(bound_data.substr(0, index), "\r\n");
	for (size_t i = 0; i < fields.size(); i++)
	{
		if (!memcmp(fields[i].c_str(), "Content-Disposition:", 20))
			this->extractContentDisposition(fields[i], name, fileName);
	}

	if (fileName.length())
	{
		fileName = "./uploads/" + fileName;
		outfile.open(fileName.c_str(),
			std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);

		if (!outfile.is_open())
		{
			std::cerr << "Request: error: can't open file " << fileName << std::endl;
			this->getErrorPage();
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
	bounds = ft_split_str(request, this->_boundary);
	for (size_t i = 0; i < bounds.size() && bounds[i] != "--\r\n"; i++)
		this->extractFile(bounds[i]);
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
			this->getErrorPage();
		if (!bytes)
		{
			this->_closeConnection = true;
			std::cout << "connection closed" << std::endl;
		}
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
		this->getErrorPage();
		return (-1);
	}

	return (0);
}

void							Request::checkBodyBytesRecieved()
{
	if (ft_stoi(this->_contentLength, NULL) != (int)this->_bodyBytesRecieved)
		this->_awaitingBody = true;
	else
	{		
		this->verifyFiles();
		this->_awaitingBody = false;
	}
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

/*
	Extract and set HTTP fields
*/

void						Request::setHTTPFields(const std::string &header)
{
	std::vector<std::string>	vct;
	std::vector<std::string>	strSplit;
	std::string					key[12] = { "GET", "HEAD", "POST", "DELETE", "Host:",
				"Connection:", "Accept:", "Referer:", "User-Agent:", "Authentification:",
				"Content-Length:", "Content-Type:"};

	vct = ft_split(header, "\n\r");
	for (size_t i = 0; i < vct.size(); i++)
		{
			strSplit = ft_split(vct[i].c_str(), " ");
			if (this->_boundarySet and strSplit[0] == this->_boundary)
				this->setGetParams(vct, &i);
			else
			{
				for (size_t j = 0; j < 12; j++)
				{
					if (strSplit[0] == key[j])
					{
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
	size_t	index;
	char	buff[BUFFLEN + 1];
	int		bytes;

	bytes = recv(fd, buff, BUFFLEN, 0);

	if (bytes < 1)
	{
		if (!bytes)
			this->_closeConnection = true;
		else if (bytes == -1)
		{
			perror("recv call failed");
			this->getErrorPage();
		}
		return (-1);
	}
	
	buff[bytes] = '\0';
	this->_request.append(buff);

	if (!memcmp(this->request.c_str(), "GET ", 3))
	{

	}

	std::cout << this->_request << std::endl;
	if ((index = this->_request.find("\r\n\r\n")) != (size_t)-1)
	{
		this->_awaitingHeader = false;
		return (bytes);
	}

	this->_awaitingHeader = true;
	return (-1);
}

void	printRequest()
{
	std::cout << "\n///////////////////////////////////////////////////////////" << std::endl;
	std::cout <<         "			R E Q U E S T"		 << std::endl;
	std::cout << "///////////////////////////////////////////////////////////" << std::endl;

}

void						Request::request(int fd)
{
	int				oct;
	size_t			index;
	std::string		header;
	std::string		body;

	oct = 0;
	printRequest();
	if (this->_awaitingBody)
	{
		this->awaitingBody(fd);
	}
	else if ((oct = this->awaitingHeader(fd)) != -1)
	{
		index = this->_request.find("\r\n\r\n");
		header = this->_request.substr(0, index);

		if (header.length())
		{
			if (header.length() >= index + 4)
				body = this->_request.substr(index + 4, this->_request.length());

			this->_bodyBytesRecieved = oct - (header.length() + 4);

			std::cout << "\n	//////	HEADER	//////\n" << header << std::endl;
			std::cout << "\n	//////	BODY	//////\n" << body << std::endl;

			this->setHTTPFields(header);

			if (this->_methodSet && this->_method == "POST")
			{
				if (body.length())
					this->bodyRequest(index);

				this->checkBodyBytesRecieved();
			}
			if (!this->_methodSet || !this->_hostSet || this->_badRequest)
				this->getErrorPage();
		}
		else 
			this->_closeConnection = true;
	}
}

std::ostream &operator<<(std::ostream & o, Request const & rhs) {
	o << "method = " << rhs.getMethod() << std::endl;
	o << "path = " << rhs.getPath() << std::endl;
	o << "httpVersion = " << rhs.getHttpVersion() << std::endl;
	o << "host = " << rhs.getHost() << std::endl;
	o << "port = " << rhs.getPort() << std::endl;
	return o;
}
