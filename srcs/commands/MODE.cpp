#include "MODE.hpp"

MODE::MODE(void): Command("MODE", 1, true) {};
MODE::~MODE(void) {};

void MODE::execute(Server* server, Client* client, IRCMessage message, std::vector<execReturnData>& execReturn) {
    execReturnData returnData = server->createBasicExecReturnData(client->getFd());

    std::string channel = message.params[0];
    if (channel[0] != '#' || message.params.size() < 2)
        return;
    channel = channel.substr(1);
    std::string mode = message.params[1];
    std::string argument = message.params.size() > 2 ? message.params[2] : "";

    Channel* selectedChannel = server->getChannelByName(channel);
    if (!selectedChannel)
        returnData.message = ERR_NOSUCHCHANNEL(client->getNickname(), channel);
    else if (!selectedChannel->hasJoined(client->getFd()))
        returnData.message = ERR_NOTONCHANNEL(client->getNickname(), channel);
    else if (!selectedChannel->isOperator(client->getFd())) {
        returnData.message = ERR_CHANOPRIVSNEEDED(client->getNickname(), channel);
    } else {
        char type = mode[0];
        char flag = mode[1];

        if (mode.length() != 2 || (type != APPEND && type != ERASE))
            returnData.message = ERR_UMODEUNKOWNFLAG(client->getNickname());
        else {
            std::string result;
            switch (flag) {
                case INVITE_ONLY_CHANNEL:
                case RESTRICTED_TOPIC_CHANNEL: {
                    result = MODE_MSG(channel, mode);
                    break;
                };

                case CHANNEL_HAS_PASSWORD: {
                    if (type == APPEND) {
                        if (argument.empty() || server->containsForbiddenChararacters(argument))
                            returnData.message = ERR_INVALIDGIVENPASSWORD(client->getNickname(), channel, mode, argument);
                        else {
                            selectedChannel->setPassword(argument);
                            result = MODE_PARAM_MSG(channel, mode, argument);
                        };
                    } else
                        result = MODE_MSG(channel, mode);
                    break;
                };

                case CHANNEL_OPERATOR_PRIVILEGE: {
                    if (!argument.length())
                        returnData.message = ERR_NEEDMOREPARAMS(client->getNickname(), "MODE");
                    else {
                        Client* selectedClient = server->getClientByNickname(argument);
                        if (!selectedClient)
                            returnData.message = ERR_NOSUCHNICK(client->getNickname(), argument);
                        else if (!selectedChannel->hasJoined(selectedClient->getFd()))
                            returnData.message = ERR_NOTONCHANNEL(client->getNickname(), channel);
                        else {
                            if (type == APPEND) {
                                if (!selectedChannel->isOperator(selectedClient->getFd()))
                                    selectedChannel->grantOperator(selectedClient->getFd());
                            } else {
                                if (selectedChannel->isOperator(selectedClient->getFd()))
                                    selectedChannel->ungrantOperator(selectedClient->getFd());
                            };
                            result = MODE_PARAM_MSG(channel, mode, argument);
                        };
                    };
                    break;
                };

                case CHANNEL_IS_USER_LIMITED: {
                    if (!argument.length())
                        returnData.message = ERR_NEEDMOREPARAMS(client->getNickname(), "MODE");
                    else {
                        if (type == APPEND) {
                            int limit = std::atoi(argument.c_str());
                            if (limit < selectedChannel->getJoinedClientsCount() || !server->isStringPositiveNumber(argument))
                                returnData.message = ERR_INVALIDLIMITNUMBER(client->getNickname(), channel, mode, argument);
                            else {
                                selectedChannel->setMaxUsers(limit);
                                result = MODE_PARAM_MSG(channel, mode, argument);
                            };
                        } else
                            result = MODE_MSG(channel, mode);
                    };
                    break;
                };

                default: {
                    returnData.message = ERR_UMODEUNKOWNFLAG(client->getNickname());
                    execReturn.push_back(returnData);
                    return;
                };
            };

            if (!result.empty()) {
                if (type == APPEND) {
                    if (!selectedChannel->hasMode(flag))
                        selectedChannel->addMode(flag);
                } else {
                    if (selectedChannel->hasMode(flag))
                        selectedChannel->rmMode(flag);
                };

                std::vector<int>& joinedChannelFds = selectedChannel->getJoinedClientsFds();
                for (std::vector<int>::iterator it = joinedChannelFds.begin(); it != joinedChannelFds.end(); it++) {
                    Client* channelClient = server->getClientByFd(*it);
                    if (channelClient) {
                        std::string& outData = channelClient->getOutData();
                        outData += result + CRLF;
                    };
                };
            };
        };
    };

    if (!returnData.message.empty())
        execReturn.push_back(returnData);
};
