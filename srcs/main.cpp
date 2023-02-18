#include "../includes/Configuration.hpp"
#include "../includes/SocketServer.hpp"
#include "CGI.hpp"

int main(int argc, char **argv, char **envp)
{
	std::string	file;
 
	(void)argv;
	(void)envp;

	if (argc > 2)
	{
		std::cout << "Error: 2 args maximum expected" << std::endl;
		return 1;
	}
	file = "configuration_file/default.conf";
	if (argc == 2)
		file = argv[1];
	Configuration	conf(file);
	std::vector<Server> test = conf.getVctServer();
	if (conf.get_errorConf())
		return 1;
	// std::cout << conf << std::endl;
	SocketServer	serv(conf, envp);
	if (serv.getErrSocket())
		return 1;
	return 0;
}

/*
	Prender en compte les methods (405 Method Not Allowed)
	Pour get et post, checker le format des args envoyes
	Voir 4096
	location / ?
	HTTP 1.1

	Arguments POST + Securisation du formulaire :
	https://www.electro-info.ovh/les-formulaires-en-PHP
	GET ne permet pas l'upload de fichiers
*/