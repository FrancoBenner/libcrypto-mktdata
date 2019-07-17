#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <chrono>
#include <thread>

#include <gtest/gtest.h>
#include <marketdata.h>

using namespace std::chrono_literals;

TEST(MarketDataClient, connect) {
    auto client = coinbase::exchange::marketdata::MarketdataClient();
    client.connect();
    client.disconnect();
}