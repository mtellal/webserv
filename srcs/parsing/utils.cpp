#include "../../includes/utils.hpp"
#include <stdlib.h>
#include <sstream>
#include <sys/stat.h>
#include <iostream>

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

std::string	ft_itos(int nbr)
{
	std::string s;
	std::stringstream out;

	out << nbr;
	s = out.str();
	return s;
}