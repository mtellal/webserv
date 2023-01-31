#include "../../includes/utils.hpp"
#include <stdlib.h>



// #include <iostream>


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
		// std::cout << "1" << std::endl;
		*err = true;
		return 0;
	}
	while ((str[i] >= '0' and str[i] <= '9') and str[i])
	{
		res = res * 10 + str[i] - 48;
		i++;
	}
	// if (str[i] != '\0')
	// {
	// 	std::cout << "2" << std::endl;
	// 	*err = true;
	// 	return 0;
	// }
	return res * sign;
}

bool	only_space_or_empty(std::string line)
{
	int i = 0;

	while (line[i] == ' ' or line[i] == '\t')
		i++;
	if (line[i] == '\0')
		return true;
	return false;
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

std::vector<std::string>	ft_split(const char *str, const char *charset)
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
}
