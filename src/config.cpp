#include <config.h>
#include <iostream>

coinbase::config::Configuration::Configuration(std::istream& in)
{
    in >> this->parsed_json;
}

coinbase::config::Configuration::~Configuration() = default;

std::vector<std::string> coinbase::config::Configuration::split(const std::string &str,
                                                                const std::regex& on_regexp) {
    std::sregex_token_iterator it{str.begin(), str.end(), on_regexp, -1};
    std::vector<std::string> words{it, {}};
    return words;
}
