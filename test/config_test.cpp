#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
#include <gtest/gtest.h>
#include <config.h>
#include <fstream>
#include <libgen.h>

using coinbase::config::Configuration;
using coinbase::config::WebSocketURI;

TEST(Configuration, load_default) {
    ASSERT_THROW({
        const Configuration *config = Configuration::load_default();
        delete config;
    }, std::runtime_error);
}

TEST(Configuration, get_has_value) {
    char *base_path = strdup(__FILE__);
    auto dir = std::string(dirname(base_path));
    auto config_path = dir + kPathSeparator + "config.json";
    std::ifstream in(config_path);
    Configuration config(in);

    auto uri = config.get<std::string>("websocket.feed.uri");
    ASSERT_EQ(std::string("wss://ws-feed.pro.coinbase.com"), uri);
}

TEST(WebSocketURI, create1) {
    WebSocketURI uri = WebSocketURI("wss://ws-feed.pro.coinbase.com");
    ASSERT_EQ(443, uri.get_port());
    ASSERT_EQ("ws-feed.pro.coinbase.com", uri.get_host());
    ASSERT_EQ("/", uri.get_path());
}

TEST(WebSocketURI, create2) {
    WebSocketURI uri = WebSocketURI("ws://ws-feed.pro.coinbase.com");
    ASSERT_EQ(80, uri.get_port());
    ASSERT_EQ("ws-feed.pro.coinbase.com", uri.get_host());
    ASSERT_EQ("/", uri.get_path());
}

TEST(WebSocketURI, create3) {
    WebSocketURI uri = WebSocketURI("ws://ws-feed.pro.coinbase.com:8080/wsapi");
    ASSERT_EQ(8080, uri.get_port());
    ASSERT_EQ("ws-feed.pro.coinbase.com", uri.get_host());
    ASSERT_EQ("/wsapi", uri.get_path());
}
