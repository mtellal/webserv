#include "utils.hpp"
#include "DefaultPage.hpp"
#include <stdlib.h>
#include <sstream>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <netdb.h>
#include <arpa/inet.h>

int	ft_stoi(const std::string str, bool *err)
{
	int	i = 0;
	int	sign = 1;
	int	res = 0;

	while ((str[i] >= '\t' && str[i] <= '\r') or str[i] == ' ')
		i++;
	if (str[i] == '+' or str[i] == '-')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	if (str[i] < '0' and str[i] > '9')
	{
		*err = true;
		return 0;
	}
	while ((str[i] >= '0' and str[i] <= '9') and str[i])
	{
		res = res * 10 + str[i] - 48;
		i++;
	}
	return res * sign;
}

bool	only_space_or_empty(std::string line)
{
	for (size_t i = 0; i < line.size(); i++)
	{
		if (line[i] != ' ' and line[i] != '\t' and line[i] != '\n')
			return false;
	}
	return true;
}

size_t	ft_belong_s(const std::string &s, const std::string &charset)
{
	size_t	i = 0;
	size_t	j = 0;

	while (i < s.length())
	{
		j = 0;
		while (j < charset.length())
		{
			if (charset[j] == s[i])
				return (charset[j]);
			j++;
		}
		i++;
	}
	return (0);
}

size_t	ft_belong_c(const char &c, const std::string &charset)
{
	size_t	i = 0;

	while (i < charset.length())
	{
		if (charset[i] == c)
			return (1);
		i++;
	}
	return (0);
}

std::vector<std::string>	ft_split(const std::string &s, const std::string &charset)
{
	std::vector<std::string> v;
	std::string str(s);
	std::string	tmp;
	int			index = 0;
	char		c;

	while ((c = ft_belong_s(str, charset)))
	{
		index = str.find(c);
		tmp = str.substr(0, index);
		if (index)
			v.push_back(str.substr(0, index));
		while (ft_belong_c(str[index], charset))
			index++;
		str.assign(str, index, str.length());
	}
	if (str.length())
		v.push_back(str);

	return (v);
}

std::vector<std::string>	ft_split_str(const std::string &src, const std::string &charset)
{
	std::vector<std::string>	v;
	std::string					s(src);
	size_t						index;
	std::string					tmp;

	while ((index = s.find(charset)) != (size_t)-1)
	{
		tmp = s.substr(0, index);
		if (tmp.length())
			v.push_back(s.substr(0, index));
		s = s.substr(index + charset.length(), s.length()); 
	}
	if (s.length())
		v.push_back(s);
	return (v);
}


std::string	ft_itos(int nbr)
{
	std::string s;
	std::stringstream out;

	out << nbr;
	s = out.str();
	return s;
}

std::string	getHttpStatusCodeMessage(int statusCode) {
	int			httpCode[9] = {200, 204, 400, 403, 404, 405, 406, 413, 500};
	std::string	message[9] = {"OK", "No Content", "Bad Request", "Forbidden", "Not Found",
							"Method Not Allowed", "Not Acceptable","Request Entity Too Large" , "Internal Server Error"};

	for (size_t i = 0; i < 9; i++)
	{
		if (httpCode[i] == statusCode)
			return message[i];
	}
	return "";
}

std::string	fileToStr(std::string path) {
	std::ifstream	file(path.c_str(), std::ios::in | std::ios::binary);
	std::string		page;

	if (file)
	{
		std::ostringstream ss;
		ss << file.rdbuf();
		page = ss.str();
		file.close();
	}
	return page;
}

std::string 	removeChar(std::string s, char c)
{
	std::string str;
	size_t		i;

	i = 0;
	while (i < s.length())
	{
		if (s[i] != c)
			str += s[i];
		i++;
	}
	return (str);
}

size_t    tab_len(char **env)
{
    size_t  i = 0;
    
    while (env && env[i])
        i++;
    return (i);
}

bool	infileExists(const std::string &file)
{
	std::ifstream in(file.c_str(), std::ifstream::in | std::ifstream::binary);

	if (!in.is_open())
		return (false);
		
	in.close();
	return (true);
}

void			trimSpaceFront(std::string &line)
{
	while (line.length() && line[0] == ' ')
		line.erase(0, 1);
}

void			trimSpaceBack(std::string &line)
{
	while (line.length() && line[line.length() - 1] == ' ')
		line.erase(line.length() - 1, 1);
}

std::string	getIPFromHostName(const std::string& hostName) {
	struct hostent* host = gethostbyname(hostName.c_str());
	if (!host)
		return "";

	std::stringstream ss;
	ss << inet_ntoa(*(struct in_addr*)host->h_addr);
	return ss.str();
}

std::string	rightRoot(Server const &serv, bool locBlockSelect, Location const &loc) {
	std::string	root;

	if (locBlockSelect and loc.getRootSet())
		root = loc.getRoot();
	else
		root = serv.getRoot();

	return root;
}

std::string	rightPathErr(bool &pageFind, int statusCode, Server const &serv, bool locBlockSelect, Location const &loc) {
	std::string									root = rightRoot(serv, locBlockSelect, loc);
	std::map<int, std::string>					mapErr;
	std::map<int, std::string>::const_iterator	it;
	std::string									rightPath;

	if (locBlockSelect and loc.getErrorPageSet())
	{
		mapErr = loc.getErrorPage();
		it = mapErr.find(statusCode);
		if (it != mapErr.end())
		{
			pageFind = true;
			rightPath = it->second;
			if (root[0] == '/')
				root.erase(0, 1);
			if (root[root.size() - 1] != '/')
				root += "/";
			root += rightPath;
			rightPath = root;
		}
	}
	if (!pageFind and serv.getErrorPageSet())
	{
		mapErr = serv.getErrorPage();
		it = mapErr.find(statusCode);
		if (it != mapErr.end())
		{
			pageFind = true;
			rightPath = it->second;
			root = serv.getRoot();
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

std::string	findRightPageError(int statusCode, Server const &serv, bool locBlockSelect, Location const &loc) {
	bool		pageFind = false;
	std::string	path;
	DefaultPage	defaultPage;

	path = rightPathErr(pageFind, statusCode, serv, locBlockSelect, loc);

	std::ifstream tmp(path.c_str(), std::ios::in | std::ios::binary);

	if (!tmp or !pageFind)
		path = defaultPage.createDefaultPage(statusCode);
	if (tmp)
		tmp.close();
	return path;
}

bool	resolveHost(const std::string& host, std::string& ipAddress) {
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_CANONNAME;

	if (getaddrinfo(host.c_str(), NULL, &hints, &res) != 0)
	{
		std::cerr << "Error: Could not resolve host \"" << host << "\"." << std::endl;
		return false;
	}
	struct sockaddr_in* addr = (struct sockaddr_in*)res->ai_addr;
	ipAddress = inet_ntoa(addr->sin_addr);
	freeaddrinfo(res);

	return true;
}
