#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <chrono>
#include <thread>

#include <gtest/gtest.h>
#include <cloudwall/crypto-mktdata/coinbase.h>

using namespace std::chrono_literals;
using namespace cloudwall::coinbase::marketdata;

TEST(Currency, init) {
    Currency ccy1 = Currency("BTC");
    Currency ccy2 = Currency("ETH");

    ASSERT_EQ("BTC", ccy1.get_ccy_code());
    ASSERT_EQ("ETH", ccy2.get_ccy_code());
    ASSERT_TRUE(ccy1 == ccy1);
    ASSERT_FALSE(ccy1 == ccy2);
}

TEST(ProductId, init) {
    Currency ccy1 = Currency("BTC");
    Currency ccy2 = Currency("ETH");
    Currency ccy3 = Currency("USD");
    ProductId p1 = ProductId(ccy1, ccy3);
    ProductId p2 = ProductId(ccy2, ccy3);

    ASSERT_EQ("BTC", p1.get_quote_ccy().get_ccy_code());
    ASSERT_EQ("ETH", p2.get_quote_ccy().get_ccy_code());
    ASSERT_EQ("USD", p1.get_base_ccy().get_ccy_code());
    ASSERT_EQ("USD", p2.get_base_ccy().get_ccy_code());
    ASSERT_TRUE(p1 == p1);
    ASSERT_FALSE(p1 == p2);
}

TEST(MarketDataClient, connect) {
    std::list<ProductId> products({
        ProductId(Currency("BTC"),Currency("USD"))
    });
    std::list<Channel> channels({
        Channel("status", { }),
        Channel("heartbeat", products),
        Channel("matches", products),
    });
    auto sub = Subscription(channels);
    int counter = 0;
    int* msg_count = &counter;
    const OnMessageCallback& callback = [msg_count](const RawFeedMessage& msg) {
        (*msg_count)++;
        ASSERT_TRUE(msg.get_raw_json().size() > 0);
    };
    auto client = RawFeedClient(sub, callback);
    client.connect();
    for (int i = 0; i < 5; i++) {
        std::this_thread::sleep_for(1s);
    }
    client.disconnect();
    ASSERT_TRUE(*msg_count > 0);
}