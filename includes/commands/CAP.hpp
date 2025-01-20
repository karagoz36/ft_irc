#pragma once
#ifndef CAP_HPP
# define CAP_HPP

# include "Command.hpp"

class CAP : public Command {
    public:
        CAP(void);
        virtual ~CAP(void);

        virtual void execute(Server* server, Client* client, IRCMessage message, std::vector<execReturnData>& execReturn);
};

#endif