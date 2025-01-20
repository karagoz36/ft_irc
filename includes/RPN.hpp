#pragma once
#ifndef RPN_HPP
# define RPN_HPP

# include <iostream>
# include <string>
# include <sstream>
# include <stack>

class RPN {
	private:
		// Orthodox Canonical Form
		RPN(void);
		RPN(const RPN& val);
		RPN& operator=(const RPN& val);
	public:
		// Orthodox Canonical Form
		~RPN(void);

		// Other
		static double doRPN(std::string input);
};

#endif