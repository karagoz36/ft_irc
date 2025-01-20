#pragma once
#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "ft_irc.hpp"

class Server;
class Command;

struct execReturnData;
struct IRCMessage;

class Client {
    private:
        int                         _fd;
        time_t                      _acceptedAt;
        bool                        _isPasswordNeeded;
        bool                        _hasUsedQuitCommand;

        std::string                 _nickname;
        std::string                 _username;
        std::string                 _hostname;
        std::string                 _servername;
        std::string                 _realname;

        std::string                 _inData;
        std::string                 _outData;
        std::vector<std::string>    _joinedChannelsNames;

        Server*                     _server;

    public:
        Client(int fd, Server* server);

        // Getters
        int const& getFd(void);
        time_t const& getAcceptedAt(void);
        bool const& isPasswordNeeded(void);
        bool const& hasUsedQuitCommand(void);
        bool isLoggedIn(void);

        std::string const& getNickname(void);
        std::string const& getUsername(void);
        std::string const& getHostname(void);
        std::string const& getServername(void);
        std::string const& getRealname(void);

        std::string getFullUserId(void);

        std::string& getInData(void);
        std::string& getOutData(void);
        std::vector<std::string>& getJoinedChannelsNames(void);

        std::vector<int> getJoinedChannelsClientsFds(void);

        Server* getServer(void);

        // Setters
        void setIsPasswordNeeded(bool isPasswordNeeded);
        void setHasUsedQuitCommand(bool hasUsedQuitCommand);
        
        void setNickname(std::string nickname);
        void setUsername(std::string username);
        void setHostname(std::string hostname);
        void setServername(std::string servername);
        void setRealname(std::string realname);

        void joinChannel(std::string name);
        void partChannel(std::string name);

        // Parser
        void parseInData(void);

        // Exec
        std::vector<execReturnData> execParsedMessage(IRCMessage parsedMessage);
};

# include "Server.hpp"
# include "Command.hpp"

#endif
