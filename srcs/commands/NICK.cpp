#include "NICK.hpp"

NICK::NICK(void): Command("NICK", 0, false) {};
NICK::~NICK(void) {};

void NICK::execute(Server* server, Client* client, IRCMessage message, std::vector<execReturnData>& execReturn) {
    execReturnData returnData = server->createBasicExecReturnData(client->getFd());

    std::string nickname = message.params.size() ? message.params[0] : "";
    std::string oldNickname = client->getNickname();

    if (nickname.empty()) {
        returnData.message = ERR_NONICKNAMEGIVEN(oldNickname);
    } else if (!server->isCorrectNickname(nickname)) {
        returnData.message = ERR_ERRONEUSNICKNAME(oldNickname, nickname);
    } else if (server->isNicknameAlreadyUsed(nickname)) {
        returnData.message = ERR_NICKNAMEINUSE(oldNickname, nickname);
    } else {
        returnData.message = RPL_NICK(client->getFullUserId(), nickname);
        client->setNickname(nickname);
        if (nickname == BOT_NICKNAME) {
            server->setBot(client);
            print_success("BOT Client set up successfully!");
        };
    };

    execReturn.push_back(returnData);
};
