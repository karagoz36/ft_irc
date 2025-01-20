#pragma once
#ifndef COMMAND_HPP
# define COMMAND_HPP

# include <string>
# include <vector>

class Server;
class Client;

struct IRCMessage;
struct execReturnData;

class Command {
    private:
        std::string     _name;
        int             _requiredParamsNumber;
        bool            _mustBeLoggedIn;

    public:
        Command(std::string name, int requiredParamsNumber, bool mustBeLogggedIn);
        virtual ~Command(void);

        // Getters
        std::string const& getName(void);
        int const& getRequiredParamsNumber(void);
        bool const& mustBeLoggedIn(void);

        // Execute
        virtual void execute(Server* server, Client* client, IRCMessage message, std::vector<execReturnData>& execReturn) = 0;
};

# include "Server.hpp"
# include "Client.hpp"
# include "ft_irc.hpp"

#endif