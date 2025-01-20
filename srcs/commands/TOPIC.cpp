#include "TOPIC.hpp"

TOPIC::TOPIC(void): Command("TOPIC", 1, true) {};
TOPIC::~TOPIC(void) {};

void TOPIC::execute(Server* server, Client* client, IRCMessage message, std::vector<execReturnData>& execReturn) {
    execReturnData returnData = server->createBasicExecReturnData(client->getFd());

    std::string channel = message.params[0].substr(1);
    std::string newTopic = message.text;

    Channel* selectedChannel = server->getChannelByName(channel);
    if (!selectedChannel)
        returnData.message = ERR_NOSUCHCHANNEL(client->getNickname(), channel);
    else if (!selectedChannel->hasJoined(client->getFd()))
        returnData.message = ERR_NOTONCHANNEL(client->getNickname(), channel);
    else if (!message.hasText) {
        std::string topic = selectedChannel->getTopic();
        if (topic.empty())
            returnData.message = RPL_NOTOPIC(client->getNickname(), channel);
        else
            returnData.message = RPL_TOPIC(client->getNickname(), channel, topic);  
    } else {
        if (selectedChannel->hasMode(RESTRICTED_TOPIC_CHANNEL) && !selectedChannel->isOperator(client->getFd()))
            returnData.message = ERR_CHANOPRIVSNEEDED(client->getNickname(), channel);
        else if (server->isCorrectTopic(newTopic)) {
            std::vector<int>& joinedChannelFds = selectedChannel->getJoinedClientsFds();
            for (std::vector<int>::iterator it = joinedChannelFds.begin(); it != joinedChannelFds.end(); it++) {
                Client* channelClient = server->getClientByFd(*it);
                if (channelClient) {
                    std::string& outData = channelClient->getOutData();
                    outData += RPL_TOPIC(client->getNickname(), channel, newTopic) + CRLF;
                };
            };
            selectedChannel->setTopic(newTopic);
        };
    };

    if (!returnData.message.empty())
        execReturn.push_back(returnData);
};
