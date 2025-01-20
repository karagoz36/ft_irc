#include "RPN.hpp"

// Orthodox Canonical Form
RPN::RPN(void) {};

RPN::RPN(const RPN& val) {
	*this = val;
};

RPN& RPN::operator=(const RPN& val) {
	(void)val;
	return *this;
};

RPN::~RPN(void) {};

// Other
double RPN::doRPN(std::string input) {
	std::stack<double> stack;
	std::istringstream tokens(input);
	std::string token;

	while (tokens >> token) {
		char c = token[0];
		if ((!isdigit(c) && c != '+' && c != '-' && c != '*' && c != '/') || token[1])
			throw std::invalid_argument("Error: Bad token.");
		else if (isdigit(c)) {
			stack.push(c - '0');
		} else {
			if (stack.size() < 2)
				throw std::invalid_argument("Error: Stack size < 2.");
			double right = stack.top(); stack.pop();
			double left = stack.top(); stack.pop();

			if (token == "+")
				stack.push(left + right);
			else if (token == "-")
				stack.push(left - right);
			else if (token == "*")
				stack.push(left * right);
			else if (token == "/")
				stack.push(left / right);
			else
				throw std::invalid_argument("Error: Bad operator.");
		};
	};

	if (stack.size() != 1)
		throw std::invalid_argument("Bad RPN formula.");
	return stack.top();
};