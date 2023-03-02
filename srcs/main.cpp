#include "Configuration.hpp"
#include "SocketServer.hpp"




int main(int argc, char **argv, char **envp)
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
	if (conf.getErrorConf())
		return 1;

	SocketServer	serv(conf, envp);
	if (serv.getErrSocket())
		return 1;

	return 0;
}

/*
	Penser a return dans la request si la valeur de retour de recv est 0 en + de
	mettre closeConnection a true.

	Pour get et post, checker le format des args envoyes
	Voir 4096

	Voir comportement nginx quand on listen avec une adress precise + 1 port, et qu'on
	essaye de se connecter sur une autre adresse avec ce meme port.

	Autres codes err ?

	Ajouter directive fichier conf -> Make the route able to accept uploaded files and
	configure where they should be saved.

	Method DELETE, code 202 ?
	Method HEAD ?

	Plusieurs fois le meme port dans le fichier conf ?
	-> Msg envoye.

	Fichier Response.cpp a clarifier

	Arguments POST + Securisation du formulaire :
	https://www.electro-info.ovh/les-formulaires-en-PHP
	GET ne permet pas l'upload de fichiers

	Tester le code 406

	favicon ?
*/
