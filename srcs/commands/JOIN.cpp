#include "JOIN.hpp"

JOIN::JOIN(void): Command("JOIN", 1, true) {};
JOIN::~JOIN(void) {};

void JOIN::execute(Server* server, Client* client, IRCMessage message, std::vector<execReturnData>& execReturn) {
    execReturnData returnData = server->createBasicExecReturnData(client->getFd());

    std::string channels = message.params[0];
    if (channels[0] != '#')
        return;
    std::string keys = message.params.size() > 1 ? message.params[1] : "";
    std::map<std::string, std::string> channelsMap = createMap(channels, keys);
    std::map<std::string, Channel>& serverChannels = server->getChannels();

    for (std::map<std::string, std::string>::iterator it = channelsMap.begin(); it != channelsMap.end(); it++) {
        std::string name = it->first;
        if (name[0] == '#') {
            name = name.substr(1);
            Channel* selectedChannel = server->getChannelByName(name);
            if (selectedChannel) {
                std::string key = it->second;

                if (selectedChannel->hasMode(CHANNEL_HAS_PASSWORD) && selectedChannel->getPassword() != key)
                    returnData.message += ERR_BADCHANNELKEY(client->getNickname(), name) + CRLF;
                else if (selectedChannel->hasMode(CHANNEL_IS_USER_LIMITED) && selectedChannel->getJoinedClientsCount() >= selectedChannel->getMaxUsers())
                    returnData.message += ERR_CHANNELISFULL(client->getNickname(), name) + CRLF;
                else if (selectedChannel->hasMode(INVITE_ONLY_CHANNEL) && !selectedChannel->isInvited(client->getFd()))
                    returnData.message += ERR_INVITEONLYCHAN(client->getNickname(), name) + CRLF;
                else {
                    selectedChannel->join(client->getFd());
                    selectedChannel->uninvite(client->getFd());
                    std::vector<int>& joinedChannelFds = selectedChannel->getJoinedClientsFds();
                    for (std::vector<int>::iterator it = joinedChannelFds.begin(); it != joinedChannelFds.end(); it++) {
                        Client* channelClient = server->getClientByFd(*it);
                        if (channelClient) {
                            std::string& outData = channelClient->getOutData();
                            outData += selectedChannel->getChannelJoinInfos(client);
                        };
                    };
                };
            } else {
                if (!server->isCorrectChannelName("#" + name))
                    returnData.message += ERR_BADCHANMASK(name) + CRLF;
                else {
                    Channel newChannel(name, server, client);
                    serverChannels.insert(std::pair<std::string, Channel>(name, newChannel));
                    returnData.message = newChannel.getChannelJoinInfos(client);
                };
            };
        };
    };

    if (!returnData.message.empty())
        execReturn.push_back(returnData);
};
