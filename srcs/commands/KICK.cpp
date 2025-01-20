#include "KICK.hpp"

KICK::KICK(void): Command("KICK", 2, true) {};
KICK::~KICK(void) {};

void KICK::execute(Server* server, Client* client, IRCMessage message, std::vector<execReturnData>& execReturn) {
    execReturnData returnData = server->createBasicExecReturnData(client->getFd());

    std::string channel = message.params[0].substr(1);
    std::string clientNickname = message.params[1];

    std::string reason = message.text.size() ? message.text : "No reason specified";

    Channel* selectedChannel = server->getChannelByName(channel);
    if (!selectedChannel)
        returnData.message = ERR_NOSUCHCHANNEL(client->getNickname(), channel);
    else if (!selectedChannel->hasJoined(client->getFd()))
        returnData.message = ERR_NOTONCHANNEL(client->getNickname(), channel);
    else if (!selectedChannel->isOperator(client->getFd()))
        returnData.message = ERR_CHANOPRIVSNEEDED(client->getNickname(), channel);
    else {
        Client* selectedClient = server->getClientByNickname(clientNickname);
        if (!selectedClient || !selectedChannel->hasJoined(selectedClient->getFd()))
            returnData.message = ERR_USERNOTINCHANNEL(client->getNickname(), clientNickname, channel);
        else {
            std::vector<int>& joinedChannelFds = selectedChannel->getJoinedClientsFds();
            for (std::vector<int>::iterator it = joinedChannelFds.begin(); it != joinedChannelFds.end(); it++) {
                Client* channelClient = server->getClientByFd(*it);
                if (channelClient) {
                    std::string& outData = channelClient->getOutData();
                    outData += RPL_KICK(client->getFullUserId(), channel, clientNickname, reason) + CRLF;
                };
            };
            selectedChannel->ungrantOperator(selectedClient->getFd());
            selectedChannel->part(selectedClient->getFd());
        };
    };

    if (!returnData.message.empty())
        execReturn.push_back(returnData);
};
