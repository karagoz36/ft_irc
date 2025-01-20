#include "USER.hpp"

USER::USER(void): Command("USER", 3, false) {};
USER::~USER(void) {};

void USER::execute(Server* server, Client* client, IRCMessage message, std::vector<execReturnData>& execReturn) {
    (void)server;
    execReturnData returnData = server->createBasicExecReturnData(client->getFd());
    
    std::string username = message.params[0];
    std::string hostname = message.params[1];
    std::string servername = message.params[2];
    std::string realname = message.text;

    if (realname.empty()) {
        returnData.message = ERR_NEEDMOREPARAMS(client->getNickname(), this->getName());
    } else if (client->isLoggedIn()) {
        returnData.message = ERR_ALREADYREGISTERED(client->getNickname());
    } else {
        client->setUsername(username);
        client->setHostname(hostname);
        client->setServername(servername);
        client->setRealname(realname);
    };

    if (!returnData.message.empty())
        execReturn.push_back(returnData);
};