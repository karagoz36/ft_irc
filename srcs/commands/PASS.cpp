#include "PASS.hpp"

PASS::PASS(void): Command("PASS", 1, false) {};
PASS::~PASS(void) {};

void PASS::execute(Server* server, Client* client, IRCMessage message, std::vector<execReturnData>& execReturn) {
    execReturnData returnData = server->createBasicExecReturnData(client->getFd());
    std::string password = message.params[0];

    if (!client->isPasswordNeeded()) {
        returnData.message = ERR_ALREADYREGISTERED(client->getNickname());
    } else if (server->getPassword() != password) {
        returnData.message = ERR_PASSWDMISMATCH(client->getNickname());
    } else
        client->setIsPasswordNeeded(false);

    if (!returnData.message.empty())
        execReturn.push_back(returnData);
};