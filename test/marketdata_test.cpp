#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <chrono>
#include <thread>

#include <gtest/gtest.h>
#include <marketdata.h>

using namespace std::chrono_literals;

TEST(MarketDataClient, connect) {
    std::list<coinbase::exchange::marketdata::ProductId> products = {};
    auto sub = coinbase::exchange::marketdata::Subscription(products);
    auto client = coinbase::exchange::marketdata::MarketdataClient(sub);
    client.connect();
    for (int i = 0; i < 10; i++) {
        std::this_thread::sleep_for(1s);
    }
    client.disconnect();
}