#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <chrono>
#include <thread>

#include <gtest/gtest.h>
#include <marketdata.h>

using namespace std::chrono_literals;
using namespace coinbase::exchange::marketdata;

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
    std::list<coinbase::exchange::marketdata::ProductId> products = {};
    auto sub = coinbase::exchange::marketdata::Subscription(products);
    auto client = coinbase::exchange::marketdata::MarketdataClient(sub);
    client.connect();
    for (int i = 0; i < 5; i++) {
        std::this_thread::sleep_for(1s);
    }
    client.disconnect();
}