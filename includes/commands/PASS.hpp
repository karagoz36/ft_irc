#pragma once
#ifndef PASS_HPP
# define PASS_HPP

# include "Command.hpp"

class PASS : public Command {
    public:
        PASS(void);
        virtual ~PASS(void);

        virtual void execute(Server* server, Client* client, IRCMessage message, std::vector<execReturnData>& execReturn);
};

#endif