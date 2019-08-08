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

#ifndef CRYPTO_MKTDATA_COINBASE_H
#define CRYPTO_MKTDATA_COINBASE_H

#include <date/date.h>

#include <cloudwall/crypto-mktdata/core.h>

using cloudwall::core::marketdata::Currency;
using cloudwall::core::marketdata::CurrencyPair;
using cloudwall::core::marketdata::OnRawFeedMessageCallback;
using cloudwall::core::marketdata::RawFeedClient;
using cloudwall::core::marketdata::Side;
using cloudwall::core::marketdata::Subscription;

/// @brief Coinbase Pro websocket API
/// @see https://docs.pro.coinbase.com/
namespace cloudwall::coinbase::marketdata {
    class CoinbaseEvent {
    public:
        enum EventType {
          match,
          status,
          ticker
        };

        virtual EventType getCoinbaseEventType() const = 0;
    };

    /// @brief callback function made every time a new event is parsed from the feed
    using OnCoinbaseEventCallback = std::function<void(const CoinbaseEvent&)>;

    /// @brief lowest-level Coinbase Pro marketdata client which feeds back raw JSON strings
    class CoinbaseRawFeedClient : public RawFeedClient {
    public:
        CoinbaseRawFeedClient(const Subscription& subscription, const OnRawFeedMessageCallback& callback);

        ~CoinbaseRawFeedClient() = default;
    };

    /// @brief higher-level Coinbase Pro marketdata client which produces typed event objects
    class CoinbaseEventClient {
    public:
        CoinbaseEventClient(const Subscription& subscription, const OnCoinbaseEventCallback& callback);

        void connect() {
            this->raw_feed_client_->connect();
        }

        void disconnect() {
            this->raw_feed_client_->disconnect();
        }

        ~CoinbaseEventClient() {
            delete this->raw_feed_client_;
        }

    private:
        CoinbaseRawFeedClient* raw_feed_client_;
    };

    /// @brief product metadata for Coinbase exchange for a particular currency pair
    class ProductStatus {
    public:
        explicit ProductStatus(rapidjson::Value::ConstValueIterator product_json);

        const std::string& get_id() const {
            return *id_;
        }

        const CurrencyPair& get_currency_pair() const {
            return *ccy_pair_;
        }

        const double get_base_min_size() const {
            return base_min_size_;
        }

        const double get_base_max_size() const {
            return base_max_size_;
        }

        const double get_base_increment() const {
            return base_increment_;
        }

        const double get_quote_increment() const {
            return quote_increment_;
        }

        const double get_min_market_funds() const {
            return min_market_funds_;
        }

        const double get_max_market_funds() const {
            return max_market_funds_;
        }

        const bool is_post_only() const {
            return post_only_;
        }

        const bool is_limit_only() const {
            return limit_only_;
        }

        const bool is_cancel_only() const {
            return cancel_only_;
        }

        const bool is_margin_enabled() const {
            return margin_enabled_;
        }

        const std::string& get_status() const {
            return *status_;
        }

        const std::string& get_status_message() const {
            return *status_message_;
        }

        ~ProductStatus();

    private:
        std::string* id_;
        CurrencyPair* ccy_pair_;
        double base_min_size_;
        double base_max_size_;
        double base_increment_;
        double quote_increment_;
        double min_market_funds_;
        double max_market_funds_;
        bool post_only_;
        bool limit_only_;
        bool cancel_only_;
        bool margin_enabled_;
        std::string* status_;
        std::string* status_message_;
    };

    /// @brief subset of the Coinbase product status message which has products but not currencies; the latter provides
    /// a lot of detail on cryptocurrencies which are view-only for the exchange, and so not as relevant
    class ProductStatusEvent : public CoinbaseEvent {
    public:
        explicit ProductStatusEvent(const std::string& raw_json);
        explicit ProductStatusEvent(const rapidjson::Document& doc);

        EventType getCoinbaseEventType() const {
            return EventType::status;
        }

        const std::list<ProductStatus*>& get_products() const {
            return *products_;
        }

        ~ProductStatusEvent();
    private:
        std::list<ProductStatus*>* products_;

        void init(const rapidjson::Document& doc);
    };

    /// @brief event fired when there is a trade match on the Coinbase exchange
    class MatchEvent : public CoinbaseEvent {
    public:
        explicit MatchEvent(const std::string& raw_json);

        EventType getCoinbaseEventType() const {
            return EventType::match;
        }

        const long get_trade_id() const {
            return trade_id_;
        }

        const long get_sequence_number() const {
            return sequence_num_;
        }

        const CurrencyPair& get_currency_pair() const {
            return *ccy_pair_;
        }

        const std::string& get_maker_order_id() const {
            return *maker_order_id_;
        }

        const std::string& get_taker_order_id() const {
            return *taker_order_id_;
        }

        const Side& get_side() const {
            return *side_;
        }

        const double get_size() const {
            return size_;
        }

        const double get_price() const {
            return price_;
        }

        const std::string& get_unparsed_timestamp() const {
            return *timestamp_txt_;
        }

        const date::sys_time<std::chrono::microseconds>& parse_timstamp() const;

        ~MatchEvent();
    private:
        long trade_id_;
        long sequence_num_;
        CurrencyPair* ccy_pair_;
        std::string* maker_order_id_;
        std::string* taker_order_id_;
        Side* side_;
        double size_;
        double price_;
        std::string* timestamp_txt_;
    };

    /// @brief event corresponding to the "ticker" channel, which has top-of-book and current volumes, 24H open/high/low
    /// and last trade price and size; this lower-frequency channel may be useful for applications that don't need
    /// to consume top N levels or full book-type marketdata
    class TickerEvent : CoinbaseEvent {
    public:
        EventType getCoinbaseEventType() const {
            return EventType::ticker;
        }

        const long get_sequence_id() const {
            return sequence_id_;
        }

        const CurrencyPair& get_currency_pair() const {
            return *ccy_pair_;
        }

        const double get_best_bid_price() const {
            return best_bid_;
        }

        const std::experimental::optional<double> get_best_bid_size() const {
            return std::experimental::nullopt;
        }

        const double get_best_ask_price() const {
            return best_ask_;
        }

        const std::experimental::optional<double> get_best_ask_size() const {
            return std::experimental::nullopt;
        }

        const double get_spread() const {
            return get_best_ask_price() - get_best_bid_price();
        }

        const long get_last_trade_id() const {
            return last_trade_id_;
        }

        const double get_last_price() const {
            return last_price_;
        }

        const double get_last_size() const {
            return last_size_;
        }

        const double get_open_24h() const {
            return open_24h_;
        }

        const double get_high_24h() const {
            return high_24h_;
        }

        const double get_low_24h() const {
            return low_24h_;
        }

        const double get_volume_24h() const {
            return volume_24h_;
        }

        const double get_volume_30d() const {
            return volume_30d_;
        }

        const std::string& get_timestamp_unparsed() const {
            return *timestamp_txt_;
        }

        const date::sys_time<std::chrono::microseconds>& parse_timstamp() const;

        ~TickerEvent();
    private:
        long sequence_id_;
        CurrencyPair* ccy_pair_;

        double best_bid_;
        double best_ask_;

        long last_trade_id_;
        double last_price_;
        double last_size_;

        double open_24h_;
        double high_24h_;
        double low_24h_;
        double volume_24h_;
        double volume_30d_;

        std::string* timestamp_txt_;
    };
}

#endif //CRYPTO_MKTDATA_COINBASE_H
