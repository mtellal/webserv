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
	size_t i = 0;

	while (i < line.length())
	{
		if (line[i] != ' ' || line[i] != '\t')
			return (false);
	}
	return (true);
}

int	ft_testchar(const char *str, const char *charset)
{
	while (*charset)
		if (*str == *charset++)
			return (1);
	return (0);
}

int	ft_lenword(const char *str, const char *charset)
{
	int	i;

	i = 0;
	while (str[i] && (ft_testchar(str + i, charset) == 0))
		i++;
	return (i);
}

int	ft_nbrword(const char *str, const char *charset)
{
	int	i;
	int	word;

	word = 0;
	while (*str)
	{
		while (*str && ft_testchar(str, charset))
			str++;
		i = ft_lenword(str, charset);
		str += i;
		if (i > 0)
			word++;
	}
	return (word);
}

/* std::vector<std::string>	ft_split(const char *str, const char *charset)
{
	std::vector<std::string>	res;
	int							i;
	int							len_arr;
	int							lenword;

	len_arr = ft_nbrword(str, charset);
	i = -1;
	while (++i < len_arr)
	{
		while (*str and ft_testchar(str, charset))
			str++;
		lenword = ft_lenword(str, charset);
		std::string src_bis(str);
		res.push_back(src_bis.substr(0, lenword));
		str += lenword;
	}
	return res;
} */

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