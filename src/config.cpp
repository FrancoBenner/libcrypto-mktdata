#include <config.h>
#include <iostream>
#include <fstream>

const char kPathSeparator =
#ifdef _WIN32
        '\\';
#else
        '/';
#endif

const coinbase::config::Configuration* coinbase::config::Configuration::load_default() {
    const char *home_dir = std::getenv("HOME");
    std::string config_file = std::string(home_dir) + kPathSeparator + ".coinbase"
            + kPathSeparator + "config.json";

    std::ifstream config_in;
    config_in.open(config_file);
    if (config_in.good()) {
        return new Configuration(config_in);
    } else {
        throw std::runtime_error("config file not found: " + config_file);
    }

}

coinbase::config::Configuration::Configuration(std::istream& in) {
    in >> this->parsed_json_;
}

coinbase::config::Configuration::~Configuration() {
    this->parsed_json_.clear();
}

std::vector<std::string> coinbase::config::Configuration::split(const std::string &str,
                                                                const std::regex& on_regexp) {
    std::sregex_token_iterator it{str.begin(), str.end(), on_regexp, -1};
    std::vector<std::string> words{it, {}};
    return words;
}
