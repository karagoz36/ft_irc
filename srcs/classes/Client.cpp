#include "Client.hpp"

Client::Client(int fd, Server* server): _fd(fd), _acceptedAt(std::time(NULL)), _isPasswordNeeded(true), _hasUsedQuitCommand(false), _nickname(DEFAULT_NAME), _username(DEFAULT_NAME), _hostname(DEFAULT_NAME), _servername(DEFAULT_NAME), _realname(DEFAULT_NAME), _server(server) {};

// Getters
int const& Client::getFd(void) {
    return this->_fd;
};

time_t const& Client::getAcceptedAt(void) {
    return this->_acceptedAt;
};

bool const& Client::isPasswordNeeded(void) {
    return this->_isPasswordNeeded;
};

bool const& Client::hasUsedQuitCommand(void) {
    return this->_hasUsedQuitCommand;
};

bool Client::isLoggedIn(void) {
    return !this->isPasswordNeeded()
           && this->getNickname() != DEFAULT_NAME
           && this->getUsername() != DEFAULT_NAME
           && this->getRealname() != DEFAULT_NAME;
};

std::string const& Client::getNickname(void) {
    return this->_nickname;
};

std::string const& Client::getUsername(void) {
    return this->_username;
};

std::string const& Client::getHostname(void) {
    return this->_hostname;
};

std::string const& Client::getServername(void) {
    return this->_servername;
};

std::string const& Client::getRealname(void) {
    return this->_realname;
};

std::string& Client::getInData(void) {
    return this->_inData;
};

std::string& Client::getOutData(void) {
    return this->_outData;
};

std::vector<std::string>& Client::getJoinedChannelsNames(void) {
    return this->_joinedChannelsNames;
};

std::vector<int> Client::getJoinedChannelsClientsFds(void) {
    Server* server = this->getServer();
    std::vector<int> joinedChannelsClientsFds;
    std::vector<std::string>& joinedChannelsNames = this->getJoinedChannelsNames();

    for (std::vector<std::string>::iterator it = joinedChannelsNames.begin(); it != joinedChannelsNames.end(); it++) {
        Channel* channel = server->getChannelByName(*it);
        if (channel) {
            std::vector<int>& channelClientsFds = channel->getJoinedClientsFds();
            for (std::vector<int>::iterator it2 = channelClientsFds.begin(); it2 != channelClientsFds.end(); it2++) {
                int clientFd = *it2;
                if (std::find(joinedChannelsClientsFds.begin(), joinedChannelsClientsFds.end(), clientFd) == joinedChannelsClientsFds.end())
                    joinedChannelsClientsFds.push_back(clientFd);
            };
        };
    };

    return joinedChannelsClientsFds;
};

Server* Client::getServer(void) {
    return this->_server;
};

// Setters
void Client::setIsPasswordNeeded(bool isPasswordNeeded) {
    this->_isPasswordNeeded = isPasswordNeeded;
};

void Client::setHasUsedQuitCommand(bool hasUsedQuitCommand) {
    this->_hasUsedQuitCommand = hasUsedQuitCommand;
};

void Client::setNickname(std::string nickname) {
    this->_nickname = nickname;
};

void Client::setUsername(std::string username) {
    this->_username = username;
};

void Client::setHostname(std::string hostname) {
    this->_hostname = hostname;
};

void Client::setServername(std::string servername) {
    this->_servername = servername;
};

void Client::setRealname(std::string realname) {
    this->_realname = realname;
};

std::string Client::getFullUserId(void) {
    return ":" + this->getNickname() + "!" + this->getUsername() + "@localhost";
};

void Client::joinChannel(std::string name) {
    std::vector<std::string>& joinedChannelsNames = this->getJoinedChannelsNames();
    joinedChannelsNames.push_back(name);
};

void Client::partChannel(std::string name) {
    std::vector<std::string>& joinedChannelsNames = this->getJoinedChannelsNames();
    std::vector<std::string>::iterator it = std::find(joinedChannelsNames.begin(), joinedChannelsNames.end(), name);
    if (it != joinedChannelsNames.end())
        joinedChannelsNames.erase(it);
};

// Parser
void Client::parseInData(void) {
    Server* server = this->getServer();
    std::string& inData = this->getInData();
    size_t pos = 0;

    while ((pos = inData.find(CRLF)) != std::string::npos) {
        std::string rawLine = inData.substr(0, pos);

        std::string token;
        std::istringstream stream(rawLine);
        IRCMessage parsedMessage;
        parsedMessage.hasText = false;

        if (!rawLine.empty() && rawLine[0] == ':') {
            std::getline(stream, token, ' ');
            parsedMessage.prefix = token.substr(1);
        };

        if (std::getline(stream, token, ' '))
            parsedMessage.command = token;

        while (std::getline(stream, token, ' ')) {
            if (!token.empty() && token[0] == ':') {
                parsedMessage.hasText = true;
                parsedMessage.text = token.substr(1);
                std::string rest;
                std::getline(stream, rest);
                if (!rest.empty())
                    parsedMessage.text += " " + rest;
                break;
            } else
                parsedMessage.params.push_back(token);
        };

        try {
            std::vector<execReturnData> data = this->execParsedMessage(parsedMessage);
            for (std::vector<execReturnData>::iterator it = data.begin(); it != data.end(); it++) {
                for (std::vector<int>::iterator it2 = it->clientsFds.begin(); it2 != it->clientsFds.end(); it2++) {
                    Client* selectedClient = server->getClientByFd(*it2);
                    if (selectedClient) {
                        std::string& outData = selectedClient->getOutData();
                        outData += it->message + CRLF;
                    };
                };
            };
        } catch (std::exception const& err) {
            print_warning(err.what());
        };

        inData.erase(0, pos + 2);
    };
};

// Exec
std::vector<execReturnData> Client::execParsedMessage(IRCMessage parsedMessage) {
    std::vector<execReturnData> returnData;
    Server* server = this->getServer();

    std::string command = parsedMessage.command;
    std::vector<std::string> params = parsedMessage.params;

    Command* selectedCommand = server->getCommandByName(command);
    
    execReturnData errorReturnData = server->createBasicExecReturnData(this->getFd());

    if (command.empty()) {
        errorReturnData.message = ERR_NEEDMOREPARAMS(this->getNickname(), command);
        print_warning("Cannot run an empty command.");
    } else if (!selectedCommand) {
        errorReturnData.message = ERR_UNKNOWNCOMMAND(this->getNickname(), command);
        print_warning("Cannot find " + command + " command.");
    } else if ((int)params.size() < selectedCommand->getRequiredParamsNumber()) {
        errorReturnData.message = ERR_NEEDMOREPARAMS(this->getNickname(), command);
        print_warning(command + " command needs at least " 
                              + getStringFromNumber(selectedCommand->getRequiredParamsNumber()) 
                              + " params (given " + getStringFromNumber(params.size()) + ").");
    } else if (!this->isLoggedIn() && selectedCommand->mustBeLoggedIn()) {
        errorReturnData.message = ERR_UNKNOWNCOMMAND(this->getNickname(), command);
        print_warning(command + " needs to be run by a logged in client.");
    };

    if (errorReturnData.message.empty()) {
        bool isLoggedInBeforeExecution = this->isLoggedIn();
        print_info("Client FD n°" + getStringFromNumber(this->getFd()) + " is doing " + command + " command!");
        selectedCommand->execute(server, this, parsedMessage, returnData);
        if (!isLoggedInBeforeExecution && this->isLoggedIn()) {
            execReturnData welcomeReturnData = server->createBasicExecReturnData(this->getFd());
            welcomeReturnData.message = RPL_WELCOME(this->getFullUserId(), this->getNickname()) + CRLF 
                                        + RPL_YOURHOST(this->getNickname()) + CRLF 
                                        + RPL_CREATED(this->getNickname(), getFormattedTime(server->getLaunchedAt())) + CRLF 
                                        + RPL_ISUPPORT(this->getNickname());
            returnData.push_back(welcomeReturnData);
        };
    } else
        returnData.push_back(errorReturnData);

    return returnData;
};
