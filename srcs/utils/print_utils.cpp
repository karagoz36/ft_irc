#include "ft_irc.hpp"

// String getters
std::string getFormattedTime(time_t _time) {
	char buffer[100];
	tm* localTime = std::localtime(&_time);
	std::strftime(buffer, sizeof(buffer), "[%m/%d/%Y • %H:%M:%S]", localTime);
	return std::string(buffer);
};

std::string getStringFromNumber(int number) {
	std::ostringstream oss;
	oss << number;
	return oss.str();
};

std::string getStringFromNumber(double number) {
	std::ostringstream oss;
	oss << number;
	return oss.str();
};

std::string getStringFromNumber(size_t number) {
	std::ostringstream oss;
	oss << number;
	return oss.str();
};

// Printers
void print_logo(void) {
	print_colored("  /$$$$$$   /$$          /$$", CYAN);
    print_colored(" /$$__  $$ | $$         |__/", CYAN);
    print_colored("| $$  \\__//$$$$$$        /$$  /$$$$$$   /$$$$$$$", CYAN);
    print_colored("| $$$$   |_  $$_/       | $$ /$$__  $$ /$$_____/", CYAN);
    print_colored("| $$_/     | $$         | $$| $$  \\__/| $$      ", CYAN);
    print_colored("| $$       | $$ /$$     | $$| $$      | $$      ", CYAN);
    print_colored("| $$       |  $$$$/     | $$| $$      |  $$$$$$$", CYAN);
    print_colored("|__/        \\___//$$$$$$|__/|__/       \\_______/", CYAN);
    print_colored("                |______/\n", CYAN);
    print_colored("   === By tkaragoz, fli & mel-habi • v1.0 ===", PURPLE);
    std::cout << std::endl;
};

void print_colored(std::string message, std::string color) {
	std::cout << BOLD << color
			  << message << END_COLOR << std::endl;
};

void print_success(std::string success) {
	std::cout << BOLD GREEN
			  << getFormattedTime(std::time(NULL))
			  << " • [SUCCESS]: " << success << END_COLOR << std::endl;
};

void print_info(std::string info) {
	std::cout << BOLD CYAN
			  << getFormattedTime(std::time(NULL))
			  << " • [INFO]: " << info << END_COLOR << std::endl;
};

void print_warning(std::string warning) {
	std::cerr << BOLD YELLOW
			  << getFormattedTime(std::time(NULL))
			  << " • [WARNING]: " << warning << END_COLOR << std::endl;
};

void print_error(std::string error) {
	std::cerr << BOLD RED 
			  << getFormattedTime(std::time(NULL)) 
			  << " • [ERROR]: " << error << END_COLOR << std::endl;
};
