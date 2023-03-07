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

	// std::cout << conf << std::endl;
	// (void)envp;

	SocketServer	serv(conf, envp);
	if (serv.getErrSocket())
		return 1;

	return 0;
}

/*
	Pour get et post, checker le format des args envoyes

	Autres codes err ?

	Method DELETE, code 202 ?

	send call failed: Broken pipe
*/
