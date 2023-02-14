#include "../includes/Configuration.hpp"
#include "../includes/SocketServer.hpp"

int main(int argc, char **argv)
{
	std::string	file;

	if (argc > 2)
	{
		std::cout << "Error: 2 args maximum expected" << std::endl;
		return 1;
	}
	file = "configuration_file/default.conf";
	if (argc == 2)
		file = argv[1];
	Configuration	conf(file);
	// std::vector<Server> test = conf.getVctServer();
	if (conf.get_errorConf())
		return 1;
	// std::cout << conf << std::endl;
	SocketServer	serv(conf);
	if (serv.getErrSocket())
		return 1;
	return 0;
}

/*
	Possible d'avoir dans le fichier de conf 2 bloc server avec le meme port et
	donc a departager avec server_name ?
	Http Methods uniquement dans bloc location ou aussi dans bloc Server ?
	On met des http_methods par default (Je pense GET par default) ?
	Cree une directive si la requete est un dossier et que autoindex est off.
	Prender en compte les methods (405 	Method Not Allowed)
	Pour get et post, checker le format des args envoyes
	Voir 4096
	location / ?

	Arguments POST + Securisation du formulaire :
	https://www.electro-info.ovh/les-formulaires-en-PHP
	GET ne permet pas l'upload de fichiers
*/