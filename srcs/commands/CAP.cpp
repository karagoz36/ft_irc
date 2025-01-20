#include "CAP.hpp"

CAP::CAP(void): Command("CAP", 1, false) {};
CAP::~CAP(void) {};

void CAP::execute(Server* server, Client* client, IRCMessage message, std::vector<execReturnData>& execReturn) {
    (void)server;
    execReturnData returnData = server->createBasicExecReturnData(client->getFd());

    std::string cmd = message.params[0];
    if (cmd == "REQ" || cmd == "DROP")
        returnData.message = "CAP * ACK :multi-prefix";
    else if (cmd == "LS" || cmd == "LIST")
        returnData.message = "CAP * LS :multi-prefix sasl away-notify";

    if (!returnData.message.empty())
        execReturn.push_back(returnData);
};