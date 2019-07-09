#include <config.h>
#include <iostream>
#include <fstream>

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

coinbase::config::WebSocketURI::WebSocketURI(const std::string &uri_txt) {
    std::smatch matches;
    if(std::regex_search(uri_txt, matches, this->uri_regex_) && matches.size() == 6) {
        std::string protocol = matches[1];
        if (protocol == "wss") {
            this->port_ = 443;
        } else {
            this->port_ = 80;
        }
        this->host_ = matches[2];
        if (matches[3].matched) {
            this->port_ = std::stoi(matches[4].str());
        }
        if (matches[5].matched) {
            this->path_ = matches[5].str();
        } else {
            this->path_ = "/";
        }
    } else {
        throw std::runtime_error("unable to recognized URI: " + uri_txt);
    }
}

coinbase::config::WebSocketURI::~WebSocketURI() = default;
