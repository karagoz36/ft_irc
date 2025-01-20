#pragma once
#ifndef USER_HPP
# define USER_HPP

# include "Command.hpp"

class USER : public Command {
    public:
        USER(void);
        virtual ~USER(void);

        virtual void execute(Server* server, Client* client, IRCMessage message, std::vector<execReturnData>& execReturn);
};

#endif