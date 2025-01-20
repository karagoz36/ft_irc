#include "INVITE.hpp"

INVITE::INVITE(void): Command("INVITE", 2, true) {};
INVITE::~INVITE(void) {};

void INVITE::execute(Server* server, Client* client, IRCMessage message, std::vector<execReturnData>& execReturn) {
    execReturnData returnData = server->createBasicExecReturnData(client->getFd());

    std::string clientNickname = message.params[0];
    std::string channel = message.params[1].substr(1);

    Channel* selectedChannel = server->getChannelByName(channel);
    if (!selectedChannel)
        returnData.message = ERR_NOSUCHCHANNEL(client->getNickname(), channel);
    else if (!selectedChannel->hasJoined(client->getFd()))
        returnData.message = ERR_NOTONCHANNEL(client->getNickname(), channel);
    else if (!selectedChannel->isOperator(client->getFd()))
        returnData.message = ERR_CHANOPRIVSNEEDED(client->getNickname(), channel);
    else {
        Client* selectedClient = server->getClientByNickname(clientNickname);
        if (!selectedClient)
            returnData.message = ERR_NOSUCHNICK(clientNickname, "#" + channel);
        else if (selectedChannel->hasJoined(selectedClient->getFd()))
            returnData.message = ERR_USERONCHANNEL(client->getNickname(), clientNickname, channel);
        else {
            std::string& outData = selectedClient->getOutData();
            outData += RPL_INVITING(client->getNickname(), clientNickname, channel) + CRLF;
            selectedChannel->invite(selectedClient->getFd());
        };
    };

    if (!returnData.message.empty())
        execReturn.push_back(returnData);
};
