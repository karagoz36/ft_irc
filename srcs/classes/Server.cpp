#include "Server.hpp"

bool Server::_isRunning = false;
int Server::_exitStatus = 0;

Server::Server(std::string port, std::string password): _botFd(-1), _sentBasicBotData(false), _bot(NULL) {
	this->_port = this->validatePort(port);
	this->_password = this->validatePassword(password);
};

// Getters
int const& Server::getFd(void) {
	return this->_fd;
};

int const& Server::getBotFd(void) {
	return this->_botFd;
};

int const& Server::getPort(void) {
	return this->_port;
};

bool const& Server::hasSentBasicBotData(void) {
	return this->_sentBasicBotData;
};

std::string const& Server::getPassword(void) {
	return this->_password;
};

time_t const& Server::getLaunchedAt(void) {
	return this->_launchedAt;
};

Client* Server::getBot(void) {
	return this->_bot;
};

std::map<int, Client>& Server::getClients(void) {
	return this->_clients;
};

std::map<std::string, Command*>& Server::getCommands(void) {
	return this->_commands;
};

std::map<std::string, Channel>& Server::getChannels(void) {
	return this->_channels;
};

std::vector<pollfd>& Server::getPollFds(void) {
	return this->_pollFds;	
};

bool Server::isNicknameAlreadyUsed(std::string nickname) {
	return !!this->getClientByNickname(nickname);
};

std::vector<int> Server::getClientsFds(void) {
	std::map<int, Client>& clients = this->getClients();
	std::vector<int> clientsFds;

	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++)
		clientsFds.push_back(it->second.getFd());

	return clientsFds;
};

bool const& Server::isRunning(void) {
	return this->_isRunning;
};

int const& Server::getExitStatus(void) {
	return this->_exitStatus;
};

pollfd Server::getServerPollFd(void) {
	pollfd serverPollFd;
	bzero(&serverPollFd, sizeof(serverPollFd));
	serverPollFd.fd = this->getFd();
	serverPollFd.events = POLLIN;
	return serverPollFd;
};

// Getters by value
Client* Server::getClientByFd(int clientFd) {
	std::map<int, Client>& clients = this->getClients();
	std::map<int, Client>::iterator it = clients.find(clientFd);
	if (it != clients.end())
		return &it->second;
	return NULL;
};

Client* Server::getClientByNickname(std::string nickname) {
	std::map<int, Client>& clients = this->getClients();
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++) {
		if (it->second.getNickname() == nickname)
			return &it->second;
	};
	return NULL;
};

Command* Server::getCommandByName(std::string name) {
	std::map<std::string, Command*>& commands = this->getCommands();
	std::map<std::string, Command*>::iterator it = commands.find(name);
	if (it != commands.end())
		return it->second;
	return NULL;
};

Channel* Server::getChannelByName(std::string name) {
	std::map<std::string, Channel>& channels = this->getChannels();
	std::map<std::string, Channel>::iterator it = channels.find(name);
	if (it != channels.end())
		return &it->second;
	return NULL;
};

// Setters
void Server::setFd(int fd) {
	this->_fd = fd;
};

void Server::setBotFd(int botFd) {
	this->_botFd = botFd;
};

void Server::setSentBasicBotData(bool sentBasicBotData) {
	this->_sentBasicBotData = sentBasicBotData;
};

void Server::setLaunchedAt(time_t launchedAt) {
	this->_launchedAt = launchedAt;
};

void Server::setBot(Client* bot) {
	this->_bot = bot;
};

void Server::setIsRunning(bool isRunning) {
	this->_isRunning = isRunning;
};

void Server::setExitStatus(int exitStatus) {
	this->_exitStatus = exitStatus;
};

void Server::setupSignals(void) {
	struct sigaction action;
	bzero(&action, sizeof(action));
	action.sa_handler = this->signalHandler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	sigaction(SIGINT, &action, NULL);
	sigaction(SIGTERM, &action, NULL);
	sigaction(SIGQUIT, &action, NULL);
	signal(SIGTSTP, SIG_IGN);
};

void Server::setupCommands(void) {
	std::map<std::string, Command*>& commands = this->getCommands();

	commands.insert(std::pair<std::string, Command*>("CAP", new CAP));
	commands.insert(std::pair<std::string, Command*>("INVITE", new INVITE));
	commands.insert(std::pair<std::string, Command*>("JOIN", new JOIN));
	commands.insert(std::pair<std::string, Command*>("KICK", new KICK));
	commands.insert(std::pair<std::string, Command*>("MODE", new MODE));
	commands.insert(std::pair<std::string, Command*>("NICK", new NICK));
	commands.insert(std::pair<std::string, Command*>("NOTICE", new NOTICE));
	commands.insert(std::pair<std::string, Command*>("PART", new PART));
	commands.insert(std::pair<std::string, Command*>("PASS", new PASS));
	commands.insert(std::pair<std::string, Command*>("PING", new PING));
	commands.insert(std::pair<std::string, Command*>("PRIVMSG", new PRIVMSG));
	commands.insert(std::pair<std::string, Command*>("QUIT", new QUIT));
	commands.insert(std::pair<std::string, Command*>("TOPIC", new TOPIC));
	commands.insert(std::pair<std::string, Command*>("USER", new USER));
};

void Server::setupBot(void) {
	int botFd = socket(AF_INET, SOCK_STREAM, 0);

	if (botFd < 0)
		throw std::runtime_error("Cannot create the BOT client.");

	sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(this->getPort());
	server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	if (connect(botFd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		close(botFd);
		throw std::runtime_error("Cannot connect the BOT Client to the server.");
	};

	this->setBotFd(botFd);
	print_colored("[BOT LOGGED IN] " BOT_NICKNAME " BOT Client logged in successfully!", GREEN);
};

void Server::rmClient(int clientFd) {
	std::map<int, Client>& clients = this->getClients();
	std::map<int, Client>::iterator it = clients.find(clientFd);
	if (it != clients.end()) {
		Client client = it->second;
		std::vector<std::string> channels = client.getJoinedChannelsNames();
		for (std::vector<std::string>::iterator it2 = channels.begin(); it2 != channels.end(); it2++) {
			Channel* channel = this->getChannelByName(*it2);
			if (channel) {
				channel->uninvite(clientFd);
				channel->ungrantOperator(clientFd);
				channel->part(clientFd);
			};
		};
		close(it->second.getFd());
		clients.erase(it);
	};
};

void Server::rmChannel(std::string name) {
	std::map<std::string, Channel>& channels = this->getChannels();
	std::map<std::string, Channel>::iterator it = channels.find(name);
	if (it != channels.end()) {
		std::vector<int> joinedClientsFds = it->second.getJoinedClientsFds();
		for (std::vector<int>::iterator it2 = joinedClientsFds.begin(); it2 != joinedClientsFds.end(); it2++) {
			Client* client = this->getClientByFd(*it2);
			if (client)
				client->partChannel(name);
		};
		channels.erase(it);
	};
};

// Static
int Server::validatePort(std::string port) {
	if (port.empty())
		throw std::invalid_argument("Port number cannot be empty.");

	for (size_t i = 0; i < port.length(); i++) {
		if (!isdigit(port[i]))
			throw std::invalid_argument("Found unexpected character in the port number: '" + std::string(&port[i]) + "'.");
	};

	int converted = std::atoi(port.c_str());
	if (converted < 0 || converted > 65535)
		throw std::invalid_argument("Port number must be between 0 and 65535 (included).");
	return converted;
};

std::string Server::validatePassword(std::string password) {
	if (password.empty())
		throw std::invalid_argument("Password cannot be empty.");

	for (size_t i = 0; i < password.length(); i++) {
		if (isspace(password[i]))
			throw std::invalid_argument("Password cannot contain a space.");
	};

	return password;
};

void Server::signalHandler(int status) {
	_isRunning = false;
	_exitStatus = 128 + status;
	std::cout << '\n';
	print_colored("[SIGNAL RECEIVED]: The server has just received an exit signal, so it will exit :)", CYAN);
};

void Server::setSocketOptions(int socketFd) {
	int opt = 1;
	setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	setsockopt(socketFd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));
};

pollfd Server::getClientPollFd(int clientFd) {
	pollfd clientPollFd;
	bzero(&clientPollFd, sizeof(clientPollFd));
	clientPollFd.fd = clientFd;
	clientPollFd.events = POLLIN | POLLOUT;
	return clientPollFd;
};

execReturnData Server::createBasicExecReturnData(int fd) {
	execReturnData result;
	result.clientsFds.push_back(fd);
	return result;
};

bool Server::isCorrectNickname(std::string nickname) {
	if (nickname.empty() || nickname.length() > 9)
		return false;

	char first = nickname[0];
	if (!std::isalpha(first) && first != '_' && first != '-' && first != '\\' 
							 && first != '[' && first != ']' && first != '{' && first != '}' && first != '|' && first != '^')
		return false;

	for (size_t i = 1; i < nickname.length(); i++) {
		char c = nickname[i];
		if (!std::isalnum(c) && c != '_' && c != '-' && c != '\\' && c != '[' 
							 && c != ']' && c != '{' && c != '}' && c != '|' && c != '^')
			return false;
	};

	return true;
};

bool Server::isCorrectChannelName(std::string name) {
	if (name.empty() || name.length() > CHANNEL_NAME_MAX_LEN || name[0] != '#')
		return false;
	return !Server::containsForbiddenChararacters(name);
};

bool Server::isCorrectTopic(std::string topic) {
	if (topic.empty() || topic.length() > TOPIC_MAX_LEN)
		return false;
	return !Server::containsForbiddenChararacters(topic);
};

bool Server::containsForbiddenChararacters(std::string str) {
	for (size_t i = 0; i < str.length(); i++) {
		char c = str[i];
		if (c <= 0x1F || c == ' ' || c == ',' || c == ':')
			return true;
	};
	return false;
};

bool Server::isStringPositiveNumber(std::string str) {
	for (size_t i = 0; i < str.length(); i++) {
		if (!isdigit(str[i]))
			return false;
	};
	return true;
};

void Server::launch(void) {
	this->setFd(socket(AF_INET, SOCK_STREAM, 0));
	int fd = this->getFd();
	if (fd < 0)
		throw std::runtime_error("The server was not able to create the socket.");

	this->setSocketOptions(fd);

	int port = this->getPort();
	sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		close(fd);
		throw std::runtime_error("The server was not able to bind the socket with the desired port.");
	};

	if (listen(fd, SOMAXCONN) < 0) {
		close(fd);
		throw std::runtime_error("The server was not able to listen to the desired socket.");
	};

	this->setIsRunning(true);
	this->setLaunchedAt(std::time(NULL));
	this->setupSignals();
	this->setupCommands();
	this->setupBot();

	print_colored("\n[SERVER LAUNCHED]: Port: " + getStringFromNumber(port) + " • " + "Password: " + this->getPassword(), PURPLE);
	print_colored("[SERVER LOGS]:", BLUE);

	int botFd = this->getBotFd();
	std::vector<pollfd>& serverPollFds = this->getPollFds();
	serverPollFds.push_back(this->getServerPollFd());
	serverPollFds.push_back(this->getClientPollFd(botFd));

	while (this->isRunning()) {
		if (poll(serverPollFds.data(), serverPollFds.size(), -1) < 0) {
			if (!this->getExitStatus()) {
				this->setExitStatus(1);
				this->setIsRunning(false);
				print_error("Error occured when calling the poll function.");
			};
			break;
		};

		for (size_t i = 0; i < serverPollFds.size(); i++) {
			pollfd* it = &serverPollFds[i];

			int pollFd = it->fd;

			if (it->revents & POLLERR) {
				int error = 1;
				socklen_t length = sizeof(error);
				this->rmClient(pollFd);
				serverPollFds.erase(serverPollFds.begin() + i--);
				if (!getsockopt(pollFd, SOL_SOCKET, SO_ERROR, &error, &length))
					print_warning("Error with the pollFd n°" + getStringFromNumber(pollFd) + ": " + strerror(error));
			} else if (it->revents & POLLOUT && pollFd == botFd) {
				if (!this->hasSentBasicBotData()) {
					std::string logString;
					logString += "PASS " + this->getPassword() + CRLF;
					logString += "NICK " BOT_NICKNAME CRLF;
					logString += "USER " BOT_USER CRLF;
					send(pollFd, logString.c_str(), logString.size(), MSG_NOSIGNAL);
					this->setSentBasicBotData(true);
				} else {
					Client* bot = this->getBot();
					if (bot) {
						std::string& inData = bot->getInData();
						size_t pos = inData.find(CRLF);
						if (pos != std::string::npos) {
							std::string message = inData.substr(0, pos + 2);
							send(pollFd, message.c_str(), message.length(), MSG_NOSIGNAL);
							inData.erase(0, pos + 2);
						};
					};
				};
			} else if (it->revents & POLLIN && pollFd == fd) {
				int clientFd = accept(fd, NULL, NULL);
				if (clientFd < 0) {
					print_warning("Cannot accept a new client (Accept < 0).");
					continue;
				} else if (serverPollFds.size() - 2 >= SOMAXCONN) {
					close(clientFd);
					print_warning("Cannot accept a new client (Too many clients).");
					continue;
				};

				this->setSocketOptions(clientFd);
				serverPollFds.push_back(this->getClientPollFd(clientFd));

				std::map<int, Client>& clients = this->getClients();
				clients.insert(std::pair<int, Client>(clientFd, Client(clientFd, this)));
				print_success("A new client with FD n°" + getStringFromNumber(clientFd) + " just arrived! Welcome to you! ^^");
			} else if (it->revents & POLLIN || it->revents & POLLOUT) {
				Client* selectedClient = this->getClientByFd(pollFd);
				if (!selectedClient)
					continue;

				if (it->revents & POLLIN) {
					char buffer[MESSAGE_SIZE];
					bzero(buffer, sizeof(buffer));

					ssize_t readBytes = recv(pollFd, buffer, sizeof(buffer) - 1, 0);
					if (readBytes <= 0) {
						this->rmClient(pollFd);
						serverPollFds.erase(serverPollFds.begin() + i--);
						print_warning("Client FD n°" + getStringFromNumber(pollFd) + " just left the server! Bye bye! ^^");
					} else {
						std::string& inData = selectedClient->getInData();
						std::string strBuffer(buffer);
						
						size_t length = strBuffer.length();
						size_t pos = strBuffer.find('\n');
						size_t pos2 = strBuffer.find('\r');

						size_t true_length = length - (pos != std::string::npos) - (pos2 != std::string::npos);
						if (pos != std::string::npos && pos == length - 1 && (pos2 == std::string::npos || pos2 != length - 2))
							strBuffer = strBuffer.substr(0, pos) + CRLF;

						inData += strBuffer;
						if (true_length > 0)
							print_info("Client FD n°" + getStringFromNumber(pollFd) 
													  + " just sent data of size " 
													  + getStringFromNumber(true_length) + ": " 
													  + (pos2 != std::string::npos ? strBuffer.substr(0, pos2) : strBuffer.substr(0, pos)).substr(0, 32)
													  + (length > 32 ? "..." : "") + "");
						selectedClient->parseInData();
					};
				} else {
					Client* bot = this->getBot();
					std::string& outData = selectedClient->getOutData();

					if (bot && selectedClient->getFd() == bot->getFd()) {
						outData.clear();
						continue;
					};

					size_t pos = outData.find(CRLF);
					if (pos != std::string::npos) {
						std::string message = outData.substr(0, pos + 2);
						send(pollFd, message.c_str(), message.length(), MSG_NOSIGNAL);
						outData.erase(0, pos + 2);
					};

					if (selectedClient->hasUsedQuitCommand() || (!selectedClient->isLoggedIn() && (std::time(NULL) - selectedClient->getAcceptedAt()) >= AUTO_LOGOUT_TIMEOUT)) {
						if (selectedClient->hasUsedQuitCommand())
							print_warning("Client FD n°" + getStringFromNumber(pollFd) + " just left the server! Bye bye! (QUIT Command) ^^");
						else
							print_warning("Client FD n°" + getStringFromNumber(pollFd) + " just left the server! Bye bye! (Auto logged out after 42s without being authentified) ^^");
						this->rmClient(pollFd);
						serverPollFds.erase(serverPollFds.begin() + i--);
					};
				};
			};
		};
	};
};

void Server::cleanup(void) {
	std::vector<pollfd>& pollFds = this->getPollFds();
	for (std::vector<pollfd>::iterator it = pollFds.begin(); it != pollFds.end(); it++)
		close(it->fd);
	pollFds.clear();

	std::map<std::string, Command*>& commands = this->getCommands();
	for (std::map<std::string, Command*>::iterator it = commands.begin(); it != commands.end(); it++) {
		if (it->second) {
			delete it->second;
			it->second = NULL;
		};
	};
	commands.clear();
	close(this->getFd());
};

void Server::runBotCommand(Client* client, Channel* channel, std::string message) {
	Client* bot = this->getBot();
	std::string& inData = bot->getInData();
	std::vector<std::string> params = split(message, ' ');
	std::string command = params[0];
	char flag = '\0';
	params.erase(params.begin());

	if (command[0] == BOT_PREFIX) {
		command = command.substr(1);
		std::string output = "Cannot find " + command + " bot command, you can check the bot commands list by using the !help command!";
		if (!channel->hasJoined(bot->getFd())) {
			if (command == "join") {
				output = RPL_JOIN(bot->getFullUserId(), channel->getName() + " " + channel->getPassword()) + CRLF;
				flag = 'J';
			};
		} else {
			if (command == "join")
				output = "I am already here ???";
			else if (command == "rpn") {
				std::string rpn;
				for (std::vector<std::string>::iterator it = params.begin(); it != params.end(); it++)
					rpn += *it + " ";

				try {
					output = "[RPN] The result of your RPN formula is: " + getStringFromNumber(RPN::doRPN(rpn)) + " !";
				} catch (std::exception const& err) {
					output = "[RPN] " + std::string(err.what());
				};
			} else if (command == "dice")
				output = "[DICE] The dice result is: " + getStringFromNumber(rand() % 6 + 1) + " !";
			else if (command == "flip") {
				output = "[FLIP] The flip result is: " + std::string(rand() % 2 ? "Heads" : "Tails") + " !";
			} else if (command == "troll") {
				output = "Segmentation fault (Core trolled)";
			} else if (command == "say") {
 				std::string say;

				for (std::vector<std::string>::iterator it = params.begin(); it != params.end(); it++)
					say += *it + " ";

				output = "[SAY] " + client->getNickname() + " says: " + say;
			} else if (command == "whoami") {
				output = "[WHOAMI] You are " + client->getNickname()
											+ (channel->isOperator(client->getFd()) ? ", an operator of this channel" : "") 
											+ ", your username is: " + client->getUsername()
											+ ", your realname is: " + client->getRealname()
											+ " and you have joined this server at: " + getFormattedTime(client->getAcceptedAt()) + ".";
			} else if (command == "part") {
				std::string reason;
				for (std::vector<std::string>::iterator it = params.begin(); it != params.end(); it++)
					reason += *it + " ";

				if (reason.empty())
					reason = "No reason specified";

				output = RPL_PART(bot->getFullUserId(), channel->getName(), reason + " (Removed by " + client->getNickname() + ")") + CRLF;
				flag = 'P';
			} else if (command == "help") {
				std::string selectedCommand;

				if (params.size() >= 1)
					selectedCommand = params[1];
				
				if (!selectedCommand.empty()) {
					output = "Cannot find " + selectedCommand + " help data.";
					if (selectedCommand == "join")
						output = "[HELP JOIN]: Usage: !join, brings the BOT to the channel.";
					else if (selectedCommand == "rpn")
						output = "[HELP RPN]: Usage: !rpn <RPN formula>, calculates the result of the RPN formula and returns it.";
					else if (selectedCommand == "dice")
						output = "[HELP DICE]: Usage: !dice, rolls a 6-faces dice and returns the result.";
					else if (selectedCommand == "flip")
						output = "[HELP FLIP]: Usage: !flip, flips a coin an returns if it is Heads or Tails.";
					else if (selectedCommand == "troll")
						output = "[HELP TROLL]: Usage: !troll, ??? :)";
					else if (selectedCommand == "say")
						output = "[HELP SAY]: Usage: !say <message>, says the message.";
					else if (selectedCommand == "whoami")
						output = "[HELP WHOAMI]: Usage: !whoami, gives informations about you.";
					else if (selectedCommand == "part")
						output = "[HELP PART]: Usage: !part, removes the BOT from the channel.";
					else if (selectedCommand == "help")
						output = "[HELP HELP]: Usage: !help <bot cmd>, returns the list of bot commands if cmd is not specified or the informations about the bot command elsewise.";
				};

				output = "[HELP] Bot commands list: join, rpn, dice, flip, troll, say, whoami, part, help.";
			};
		};

		if (flag != 'P' && flag != 'J')
			output = RPL_NOTICE(bot->getFullUserId(), "#" + channel->getName(), client->getNickname() + ": " + output) + CRLF;
		if (flag == 'J' || channel->hasJoined(bot->getFd())) {
			if (flag == 'P')
				inData += RPL_NOTICE(bot->getFullUserId(), "#" + channel->getName(), "Bye bye everyone!") + CRLF;
			inData += output;
			if (flag == 'J')
				inData += RPL_NOTICE(bot->getFullUserId(), "#" + channel->getName(), client->getNickname() + ": Thank you for adding me in your channel, you can do the !help command to get my commands!") + CRLF;
		};
	};
};
