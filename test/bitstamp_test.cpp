#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <chrono>
#include <thread>

#include <gtest/gtest.h>
#include <cloudwall/crypto-mktdata/bitstamp.h>

using namespace std::chrono_literals;
using namespace cloudwall::bitstamp::marketdata;

using cloudwall::core::marketdata::Channel;

TEST(BitstampRawFeedClient, connect) {
    auto ccy_pair = CurrencyPair(Currency("BTC"), Currency("USD"));
    std::list<Channel> channels ({
        Channel("live_trades", ccy_pair),
        Channel("live_orders", ccy_pair)
    });
    auto sub = Subscription(channels);
    int counter = 0;
    int* msg_count = &counter;
    const OnRawFeedMessageCallback& callback = [msg_count](const RawFeedMessage& msg) {
        (*msg_count)++;
        spdlog::info("Incoming message: {}", msg.get_raw_json());
        ASSERT_FALSE(msg.get_raw_json().empty());
    };
    auto client = RawFeedClient(sub, callback);
    client.connect();
    for (int i = 0; i < 5; i++) {
        std::this_thread::sleep_for(1s);
    }
    client.disconnect();
    ASSERT_TRUE(*msg_count > 0);
}