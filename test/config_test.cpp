#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
#include <gtest/gtest.h>
#include <config.h>
#include <fstream>

using coinbase::config::Configuration;

TEST(Configuration, load_default) {
    ASSERT_THROW({
        const Configuration *config = Configuration::load_default();
        delete config;
    }, std::runtime_error);
}

TEST(Configuration, get_has_value) {
    std::ifstream in("config.json");
    Configuration config(in);

    auto port = config.get<int>("api.port");
    ASSERT_EQ(8080, port.value());
}

TEST(Configuration, get_missing_value1) {
    std::ifstream in("config.json");
    Configuration config(in);

    auto port = config.get<int>("no_api.port");
    ASSERT_FALSE(port);
}

TEST(Configuration, get_missing_value2) {
    std::ifstream in("config.json");
    Configuration config(in);

    auto port = config.get<int>("api.no_port");
    ASSERT_FALSE(port);
}
