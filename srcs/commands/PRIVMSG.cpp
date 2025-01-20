#include "PRIVMSG.hpp"

PRIVMSG::PRIVMSG(void): Command("PRIVMSG", 0, true) {};
PRIVMSG::~PRIVMSG(void) {};

void PRIVMSG::execute(Server* server, Client* client, IRCMessage message, std::vector<execReturnData>& execReturn) {
    execReturnData returnData = server->createBasicExecReturnData(client->getFd());
    std::string msg = message.text;

    if (msg.empty())
        returnData.message = ERR_NOTEXTTOSEND(client->getNickname());
    else if (!message.params.size()) {
        returnData.message = ERR_NORECIPIENT(client->getNickname()) + CRLF;
    } else {
        std::vector<std::string> wantedChannels = split(message.params[0], ',');
        Client* bot = server->getBot();

        for (std::vector<std::string>::iterator it = wantedChannels.begin(); it != wantedChannels.end(); it++) {
            std::string destination = *it;
            if (destination[0] == '#') {
                destination = destination.substr(1);
                Channel* destinationChannel = server->getChannelByName(destination);
                if (!destinationChannel)
                    returnData.message += ERR_NOSUCHNICK(client->getNickname(), "#" + destination) + CRLF;
                else if (!destinationChannel->hasJoined(client->getFd()))
                    returnData.message += ERR_NOTONCHANNEL(client->getNickname(), "#" + destination) + CRLF;
                else {
                    std::vector<int>& joinedChannelFds = destinationChannel->getJoinedClientsFds();
                    for (std::vector<int>::iterator it = joinedChannelFds.begin(); it != joinedChannelFds.end(); it++) {
                        Client* channelClient = server->getClientByFd(*it);
                        if (channelClient && channelClient->getFd() != client->getFd()) {
                            std::string& outData = channelClient->getOutData();
                            outData += RPL_PRIVMSG(client->getFullUserId(), "#" + destination, msg) + CRLF;
                        };
                    };
                    if (bot)
                        server->runBotCommand(client, destinationChannel, msg);
                };
            } else {
                Client* destinationClient = server->getClientByNickname(destination);
                if (!destinationClient)
                    returnData.message += ERR_NOSUCHNICK(client->getNickname(), destination) + CRLF;
                else {
                    std::string& outData = destinationClient->getOutData();
                    outData += RPL_PRIVMSG(client->getFullUserId(), destination, msg) + CRLF;
                };
            };
        };
    };

    if (!returnData.message.empty())
        execReturn.push_back(returnData);
};