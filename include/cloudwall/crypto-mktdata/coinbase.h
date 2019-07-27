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

#include <fmt/core.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXSocket.h>
#include <rapidjson/pointer.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>
#include <spdlog/spdlog.h>

#include <cloudwall/crypto-mktdata/core.h>

using cloudwall::core::marketdata::Currency;

namespace cloudwall::coinbase::marketdata {
    class ProductId {
    public:
        ProductId(const Currency& quote_ccy, const Currency& base_ccy);

        [[nodiscard]] const Currency& get_base_ccy() const {
            return base_ccy_;
        }

        [[nodiscard]] const Currency& get_quote_ccy() const {
            return quote_ccy_;
        }

        bool operator == (const ProductId &other) const {
            return (this->get_base_ccy() == other.get_base_ccy())
                && (this->get_quote_ccy() == other.get_quote_ccy());
        }

        ~ProductId() = default;
    private:
        const Currency& base_ccy_;

        const Currency& quote_ccy_;
    };

    std::ostream& operator << (std::ostream& out, const ProductId& product_id);

    class Channel {
    public:
        Channel(const std::string& name, const std::list<ProductId>& product_ids);

        [[nodiscard]] const std::string& get_name() const {
            return name_;
        }

        [[nodiscard]] const std::list<ProductId>& get_product_ids() const {
            return product_ids_;
        }
    private:
        const std::string& name_;
        const std::list<ProductId>& product_ids_;
    };

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

    std::ostream& operator << (std::ostream& out, const Subscription& subscription);

    class MarketdataMessage {
    public:
        MarketdataMessage(const std::string& type, const rapidjson::Document& document);

        [[nodiscard]] const std::string& get_type() const {
            return type_;
        }

        [[nodiscard]] const rapidjson::Document& get_document() const {
            return document_;
        }

        ~MarketdataMessage();
    private:
        const std::string& type_;
        const rapidjson::Document& document_;
    };

    using OnMessageCallback = std::function<void(const MarketdataMessage&)>;

    class MarketdataClient {
    public:
        MarketdataClient(const Subscription& subscription, const OnMessageCallback& callback);

        void connect();

        void disconnect();

        ~MarketdataClient();
    private:
        ix::WebSocket *websocket_;
        const OnMessageCallback& callback_;
    };
}

#endif //CRYPTO_MKTDATA_COINBASE_H
