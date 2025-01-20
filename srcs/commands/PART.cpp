#include "PART.hpp"

PART::PART(void): Command("PART", 1, true) {};
PART::~PART(void) {};

void PART::execute(Server* server, Client* client, IRCMessage message, std::vector<execReturnData>& execReturn) {
    execReturnData returnData = server->createBasicExecReturnData(client->getFd());

    std::string channels = message.params[0];
    std::string reason = message.text.length() ? message.text : "No reason specified";
    std::vector<std::string> wantedChannels = split(channels, ',');

    for (std::vector<std::string>::iterator it = wantedChannels.begin(); it != wantedChannels.end(); it++) {
        std::string name = *it;
        if (name[0] == '#') {
            name = name.substr(1);
            Channel* destinationChannel = server->getChannelByName(name);
            if (!destinationChannel)
                returnData.message += ERR_NOSUCHNICK(client->getNickname(), "#" + name) + CRLF;
            else if (!destinationChannel->hasJoined(client->getFd()))
                returnData.message += ERR_NOTONCHANNEL(client->getNickname(), name) + CRLF;
            else {
                std::vector<int>& joinedChannelFds = destinationChannel->getJoinedClientsFds();
                for (std::vector<int>::iterator it = joinedChannelFds.begin(); it != joinedChannelFds.end(); it++) {
                    Client* channelClient = server->getClientByFd(*it);
                    if (channelClient) {
                        std::string& outData = channelClient->getOutData();
                        outData += RPL_PART(client->getFullUserId(), name, reason) + CRLF;
                    };
                };
                destinationChannel->ungrantOperator(client->getFd());
                destinationChannel->part(client->getFd());
            };
        };
    };

    if (!returnData.message.empty())
        execReturn.push_back(returnData);
};