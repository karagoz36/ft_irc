#include "QUIT.hpp"

QUIT::QUIT(void): Command("QUIT", 0, false) {};
QUIT::~QUIT(void) {};

void QUIT::execute(Server* server, Client* client, IRCMessage message, std::vector<execReturnData>& execReturn) {
    (void)server;
    execReturnData returnData;
    std::vector<int> joinedChannelsClientsFds = client->getJoinedChannelsClientsFds();
    std::string reason = message.text.length() ? message.text : "No reason specified";

    returnData.clientsFds.insert(returnData.clientsFds.end(), joinedChannelsClientsFds.begin(), joinedChannelsClientsFds.end());
    if (std::find(returnData.clientsFds.begin(), returnData.clientsFds.end(), client->getFd()) == returnData.clientsFds.end())
        returnData.clientsFds.push_back(client->getFd());
    returnData.message = RPL_QUIT(client->getFullUserId(), reason);

    client->setHasUsedQuitCommand(true);
    execReturn.push_back(returnData);
};