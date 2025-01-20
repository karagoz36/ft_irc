#include "ft_irc.hpp"

std::vector<std::string> split(std::string const& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream stream(str);
    while (std::getline(stream, token, delimiter))
        tokens.push_back(token);
    return tokens;
};

std::map<std::string, std::string> createMap(std::string const& str1, std::string const& str2) {
    std::map<std::string, std::string> result;
    std::vector<std::string> tokens1 = split(str1, ',');
    std::vector<std::string> tokens2 = split(str2, ',');

    if (tokens1.empty())
        return result;
    else if (tokens2.empty())
        tokens2.push_back("");

    for (size_t i = 0; i < tokens1.size(); i++) {
        std::string key = tokens1[i];
        std::string value = (i < tokens2.size()) ? tokens2[i] : tokens2.back();
        result.insert(std::pair<std::string, std::string>(key, value));
    };
    
    return result;
};