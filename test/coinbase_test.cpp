// Copyright 2019 Kyle F. Downey
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <chrono>
#include <thread>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <gtest/gtest.h>

#include <cloudwall/crypto-mktdata/coinbase.h>

using namespace std::chrono_literals;

using cloudwall::coinbase::marketdata::CoinbaseEvent;
using cloudwall::coinbase::marketdata::CoinbaseEventClient;
using cloudwall::coinbase::marketdata::CoinbaseRawFeedClient;
using cloudwall::coinbase::marketdata::OnCoinbaseEventCallback;
using cloudwall::coinbase::marketdata::ProductStatusEvent;
using cloudwall::core::marketdata::Channel;
using cloudwall::core::marketdata::Currency;
using cloudwall::core::marketdata::CurrencyPair;
using cloudwall::core::marketdata::RawFeedMessage;

TEST(CoinbaseProRawFeedClient, connect) {
    auto ccy_pair = CurrencyPair(Currency("BTC"), Currency("USD"));
    std::list<Channel> channels({
        Channel("status", { }),
        Channel("matches", ccy_pair),
        Channel("ticker", ccy_pair)
    });
    auto sub = Subscription(channels);
    int counter = 0;
    int* msg_count = &counter;
    const OnRawFeedMessageCallback& callback = [msg_count](const RawFeedMessage& msg) {
        (*msg_count)++;
        spdlog::info("Incoming message: {}", msg.get_raw_json());
        ASSERT_FALSE(msg.get_raw_json().empty());
    };
    auto client = CoinbaseRawFeedClient(sub, callback);
    client.connect();
    for (int i = 0; i < 5; i++) {
        std::this_thread::sleep_for(1s);
    }
    client.disconnect();
    ASSERT_TRUE(*msg_count > 0);
}

TEST(ProductStatusEvent, parse) {
    boost::filesystem::path test_path(__FILE__);
    boost::filesystem::path json_path = test_path.remove_filename().append("example_status_msg.json");
    boost::filesystem::ifstream ifs(json_path);

    std::stringstream sstr;
    sstr << ifs.rdbuf();
    auto event = ProductStatusEvent(sstr.str());
    auto first = event.get_products().front();

    ASSERT_EQ(48, event.get_products().size());
    ASSERT_EQ("ZEC-BTC", first->get_id());
    ASSERT_EQ("ZEC", first->get_currency_pair().get_base_ccy().get_ccy_code());
    ASSERT_DOUBLE_EQ(0.01, first->get_base_min_size());
    ASSERT_DOUBLE_EQ(1500, first->get_base_max_size());
    ASSERT_DOUBLE_EQ(0.0001, first->get_base_increment());
    ASSERT_DOUBLE_EQ(0.000001, first->get_quote_increment());
    ASSERT_DOUBLE_EQ(0.001, first->get_min_market_funds());
    ASSERT_DOUBLE_EQ(30, first->get_max_market_funds());
    ASSERT_FALSE(first->is_margin_enabled());
    ASSERT_FALSE(first->is_cancel_only());
    ASSERT_TRUE(first->is_limit_only());
    ASSERT_FALSE(first->is_post_only());
    ASSERT_EQ("online", first->get_status());
    ASSERT_EQ("", first->get_status_message());
}

TEST(CoinbaseEventClient, product_status_only) {
    auto ccy_pair = CurrencyPair(Currency("BTC"), Currency("USD"));
    std::list<Channel> channels({
            Channel("status", { })
    });
    auto sub = Subscription(channels);
    int counter = 0;
    int* msg_count = &counter;
    const OnCoinbaseEventCallback& callback = [msg_count](const CoinbaseEvent& event) {
      if (CoinbaseEvent::EventType::status == event.getCoinbaseEventType()) {
          (*msg_count)++;
          const ProductStatusEvent& specific = static_cast<const ProductStatusEvent&>(event);
          ASSERT_TRUE(specific.get_products().size() > 0);
      } else {
          FAIL();
      }
    };
    auto client = CoinbaseEventClient(sub, callback);
    client.connect();
    for (int i = 0; i < 5; i++) {
        std::this_thread::sleep_for(1s);
    }
    client.disconnect();
    ASSERT_TRUE(*msg_count > 0);
}
