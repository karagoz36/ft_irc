#pragma once
#ifndef SERVER_HPP
# define SERVER_HPP

# include "ft_irc.hpp"

class Client;
class Channel;
class Command;

struct execReturnData;

class Server {
	private:
		int			_fd;
		int			_botFd;
		int			_port;
		bool		_sentBasicBotData;
		std::string	_password;
		time_t		_launchedAt;
		Client*		_bot;

		std::map<int, Client> _clients;
		std::map<std::string, Command*> _commands;
		std::map<std::string, Channel> _channels;
		std::vector<pollfd> _pollFds;

		static bool	_isRunning;
		static int	_exitStatus;

	public:
		Server(std::string port, std::string password);

		// Getters
		int const& getFd(void);
		int const& getBotFd(void);
		int const& getPort(void);
		bool const& hasSentBasicBotData(void);
		std::string const& getPassword(void);
		time_t const& getLaunchedAt(void);
		Client*	getBot(void);

		std::map<int, Client>& getClients(void);
		std::map<std::string, Command*>& getCommands(void);
		std::map<std::string, Channel>& getChannels(void);
		std::vector<pollfd>& getPollFds(void);

		bool const& isRunning(void);
		int const& getExitStatus(void);

		pollfd getServerPollFd(void);

		bool isNicknameAlreadyUsed(std::string nickname);

		std::vector<int> getClientsFds(void);

		// Getters by value
		Client* getClientByFd(int clientFd);
		Client* getClientByNickname(std::string nickname);

		Command* getCommandByName(std::string name);

		Channel* getChannelByName(std::string name);
 
		// Setters
		void setFd(int fd);
		void setBotFd(int botFd);
		void setSentBasicBotData(bool sentBasicBotData);
		void setLaunchedAt(time_t launchedAt);
		void setBot(Client* bot);

		void setIsRunning(bool isRunning);
		void setExitStatus(int exitStatus);
		void setupSignals(void);
		void setupCommands(void);
		void setupBot(void);

		void rmClient(int clientFd);
		void rmChannel(std::string name);

		// Static
		static int validatePort(std::string port);
		static std::string validatePassword(std::string password);
		static void signalHandler(int status);
		static void setSocketOptions(int socketFd);
		static pollfd getClientPollFd(int clientFd);

		static execReturnData createBasicExecReturnData(int fd);
	
		static bool isCorrectNickname(std::string nickname);
		static bool isCorrectChannelName(std::string name);
		static bool isCorrectTopic(std::string topic);
		static bool containsForbiddenChararacters(std::string str);
		static bool isStringPositiveNumber(std::string str);

		// Launcher
		void launch(void);
		void cleanup(void);
		void runBotCommand(Client* client, Channel* channel, std::string message);
};

# include "Channel.hpp"
# include "Client.hpp"

#endif
