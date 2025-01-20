#include "ft_irc.hpp"

int main(int argc, char** argv) {
	print_logo();

	if (argc != 3) {
		print_colored("[SERVER ERROR]: Please provide at least 2 arguments, ./ircserv [PORT] [PASSWORD]", RED);
		print_colored("[SERVER INFO]: For example: ./ircserv 4242 FortyTwo", CYAN);
		return 1;
	};

	std::string port = argv[1];
	std::string password = argv[2];

	try {
		srand(static_cast<unsigned int>(time(NULL)));
		Server server(port, password);
		server.launch();
		server.cleanup();
		print_colored("[SERVER EXITED]: Bye bye!", GREEN);
		return server.getExitStatus();
	} catch (std::exception const& err) {
		print_colored("[SERVER ERROR]: " + std::string(err.what()), RED);
		return 1;
	};

	return 0;
};
