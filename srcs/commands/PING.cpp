#include "PING.hpp"

PING::PING(void): Command("PING", 1, false) {};
PING::~PING(void) {};

void PING::execute(Server* server, Client* client, IRCMessage message, std::vector<execReturnData>& execReturn) {
    execReturnData returnData = server->createBasicExecReturnData(client->getFd());
    returnData.message = RPL_PONG(client->getFullUserId(), message.params[0]);
    execReturn.push_back(returnData);
};