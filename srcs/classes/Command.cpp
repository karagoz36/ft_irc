#include "Command.hpp"

Command::Command(std::string name, int requiredParamsNumber, bool mustBeLoggedIn): _name(name), _requiredParamsNumber(requiredParamsNumber), _mustBeLoggedIn(mustBeLoggedIn) {
    print_colored("[COMMAND LOADED] " + name + " command loaded successfully!", CYAN);
};

Command::~Command(void) {};

// Getters
std::string const& Command::getName(void) {
    return this->_name;
};

int const& Command::getRequiredParamsNumber(void) {
    return this->_requiredParamsNumber;
};

bool const& Command::mustBeLoggedIn(void) {
    return this->_mustBeLoggedIn;
};