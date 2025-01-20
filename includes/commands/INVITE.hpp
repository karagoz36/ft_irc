#pragma once
#ifndef INVITE_HPP
# define INVITE_HPP

# include "Command.hpp"

class INVITE : public Command {
    public:
        INVITE(void);
        virtual ~INVITE(void);

        virtual void execute(Server* server, Client* client, IRCMessage message, std::vector<execReturnData>& execReturn);
};

#endif