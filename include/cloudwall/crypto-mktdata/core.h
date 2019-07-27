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

#ifndef CRYPTO_MKTDATA_CORE_H
#define CRYPTO_MKTDATA_CORE_H

#include <experimental/optional>
#include <list>
#include <ostream>

#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXSocket.h>

namespace cloudwall::core::marketdata {
    /// @brief a reference to a cryptocurrency (e.g. BTC) or fiat currency (e.g. USD)
    class Currency {
    public:
        explicit Currency(const std::string &ccy_code);

        [[nodiscard]] const std::string &get_ccy_code() const {
            return ccy_code_;
        }

        bool operator==(const Currency &other) const {
            return (this->get_ccy_code() == other.get_ccy_code());
        }

        ~Currency() = default;

    private:
        const std::string &ccy_code_;
    };

    /// @brief a reference to an exchange-traded currency pair
    class CurrencyPair {
    public:
        CurrencyPair(const Currency& quote_ccy, const Currency& base_ccy);

        [[nodiscard]] const Currency& get_base_ccy() const {
            return base_ccy_;
        }

        [[nodiscard]] const Currency& get_quote_ccy() const {
            return quote_ccy_;
        }

        bool operator == (const CurrencyPair &other) const {
            return (this->get_base_ccy() == other.get_base_ccy())
                   && (this->get_quote_ccy() == other.get_quote_ccy());
        }

        ~CurrencyPair() = default;
    private:
        const Currency& base_ccy_;

        const Currency& quote_ccy_;
    };

    /// @brief a channel or topic on the exchange feed that you can subscribe to, optionally qualified by currency pair
    class Channel {
    public:
        explicit Channel(const std::string& name,
                         const std::experimental::optional<CurrencyPair>& product_id = std::experimental::nullopt);

        [[nodiscard]] const std::string& get_name() const {
            return name_;
        }

        [[nodiscard]] const std::experimental::optional<CurrencyPair>& get_ccy_pair() const {
            return ccy_pair_;
        }
    private:
        const std::string& name_;
        const std::experimental::optional<CurrencyPair>& ccy_pair_;
    };

    /// @brief a composite subscription to one or more channels
    class Subscription {
    public:
        explicit Subscription(const std::list<Channel>& channels);

        [[nodiscard]] const std::list<Channel>& get_channels() const {
            return channels_;
        }

        ~Subscription() = default;
    private:
        const std::list<Channel>& channels_;
    };

    /// @brief wrapper around a raw Websocket feed message, typically in JSON
    class RawFeedMessage {
    public:
        explicit RawFeedMessage(const std::string& raw_json);

        [[nodiscard]] const std::string& get_raw_json() const {
            return raw_json_;
        }

        ~RawFeedMessage();
    private:
        const std::string& raw_json_;
    };

    /// @brief callback function made every time a new message is received on a websocket channel
    using OnRawFeedMessageCallback = std::function<void(const RawFeedMessage&)>;

    /// @brief common interface for a raw message feed client based on websockets
    class RawFeedClient {
    public:
        RawFeedClient(ix::WebSocket* websocket, const OnRawFeedMessageCallback& callback)
            : websocket_(websocket), callback_(callback) { }

        void connect() {
            websocket_->start();
        }

        void disconnect() {
            websocket_->stop();
        }

        ~RawFeedClient() {
            delete websocket_;
        }
    protected:
        ix::WebSocket *websocket_;
        const OnRawFeedMessageCallback& callback_;
    };
}

#endif //CRYPTO_MKTDATA_CORE_H
