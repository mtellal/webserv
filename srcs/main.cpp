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
	-Check les valeur de read / recv / send / write (0 ET -1)
	et un seul appel a une de ces fonctions par epoll_wait;

	-Pas le droit de check la valeur de errno juste apres l'appel d'une de ces
	fonctions ci dessus

	-"poll() (or equivalent) must check read and write at the same time."
	donc EPOLLIN ET EPOLLOUT ?

	-Leaks avec form

	send call failed: Broken pipe
*/
