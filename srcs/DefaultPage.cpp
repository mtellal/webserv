/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DefaultPage.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtellal <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/18 18:19:23 by mtellal           #+#    #+#             */
/*   Updated: 2023/02/18 18:19:35 by mtellal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DefaultPage.hpp"

DefaultPage::DefaultPage() {}

DefaultPage::DefaultPage(const Request &r, const Server &s): _req(r), _serv(s) {}

DefaultPage::DefaultPage(const DefaultPage &page) { *this = page; }

DefaultPage::~DefaultPage() {}

DefaultPage    &DefaultPage::operator=(const DefaultPage &page) 
{
    if (this == &page)
    {
        this->_req = page._req;
		this->_serv = page._serv;
	}
    return (*this);
}

/* Fct tmp qui me sert juste pour des tests, va etre suprimee */
// std::string		DefaultPage::argsToStr() {
// 	std::map<std::string, std::string>	args;
// 	std::string							res;

// 	args = this->_req.getQueryString();

// 	res += args["titre"];
// 	res += " ";
// 	res += args["nom"];
// 	res += " ";
// 	res += args["prenom"];
// 	res += " a ";
// 	res += args["age"];
// 	res += " ans, ";
// 	if (args["titre"] == "M.")
// 		res += "il ";
// 	else
// 		res += "elle ";
// 	if (args["bDebutant"] == "on")
// 		res += "debute en php";
// 	else
// 		res += "est pro en php";

// 	return res;
// }

// std::string     DefaultPage::createResFormPage() {
// 	std::ofstream file("/tmp/tmpFile.html", std::ios::out | std::ios::trunc);

// 	file << "<!DOCTYPE html>" << std::endl;
// 	file << "<html lang=\"en\">" << std::endl;
// 	file << "<head>" << std::endl;
// 	file << "	<meta charset=\"UTF-8\">" << std::endl;
// 	file << "	<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">" << std::endl;
// 	file << "	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
// 	file << "	<title>Form</title>" << std::endl;
// 	file << "</head>" << std::endl;
// 	file << "<body>" << std::endl;
// 	file << "	<p>" + this->argsToStr() + "</p>"<< std::endl;
// 	file << "</body>" << std::endl;
// 	file << "</html>" << std::endl;
// 	file.close();

// 	return "/tmp/tmpFile.html";
// }

std::string     DefaultPage::createDefaultPage(int statusCode) {
	std::ofstream file("/tmp/tmpFile.html", std::ios::out | std::ios::trunc);

	file << "<!DOCTYPE html>" << std::endl;
	file << "<html lang=\"en\">" << std::endl;
	file << "<head>" << std::endl;
	file << "	<meta charset=\"UTF-8\">" << std::endl;
	file << "	<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">" << std::endl;
	file << "	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
	file << "	<title>Webserv " + ft_itos(statusCode) + "</title>" << std::endl;
	file << "</head>" << std::endl;
	file << "<body>" << std::endl;
	file << "	<h1>" + ft_itos(statusCode) + ", " + getHttpStatusCodeMessage(statusCode) + "</h1>" << std::endl;
	file << "</body>" << std::endl;
	file << "</html>" << std::endl;
	file.close();

	return "/tmp/tmpFile.html";
}

/*	Cree un lien pour chaque dossier / fichier*/
void            DefaultPage::fileAndDir(std::ofstream &file, bool getDir, std::string path) {
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
					file << "<a href=\"http://" + this->_serv.getHost() + ":" + this->_serv.getPort() +
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

/*	Affiche l'autoindex, deja les dossiers puis les ficheirs (comme nginx) */
std::string     DefaultPage::createAutoindexPage(std::vector<std::string> p) {
	std::string		path = p[0];
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