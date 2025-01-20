#pragma once
#ifndef NOTICE_HPP
# define NOTICE_HPP

# include "Command.hpp"

class NOTICE : public Command {
    public:
        NOTICE(void);
        virtual ~NOTICE(void);

        virtual void execute(Server* server, Client* client, IRCMessage message, std::vector<execReturnData>& execReturn);
};

#endif
