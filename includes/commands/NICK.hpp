#pragma once
#ifndef NICK_HPP
# define NICK_HPP

# include "Command.hpp"

class NICK : public Command {
    public:
        NICK(void);
        virtual ~NICK(void);

        virtual void execute(Server* server, Client* client, IRCMessage message, std::vector<execReturnData>& execReturn);
};

#endif