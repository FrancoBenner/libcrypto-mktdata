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

#ifndef LIBCOINBASE_WSAPI_H
#define LIBCOINBASE_WSAPI_H

#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXSocket.h>
#include <nlohmann/json.hpp>

namespace coinbase::exchange::marketdata {
    class Currency {
    public:
        Currency(const std::string& ccy_code);

        const std::string& get_ccy_code() const {
            return ccy_code_;
        }

        bool operator == (const Currency &other) const {
            return(this->get_ccy_code() == other.get_ccy_code());
        }

        ~Currency() = default;
    private:
        const std::string& ccy_code_;
    };

    std::ostream& operator << (std::ostream& out, const Currency& ccy);

    class ProductId {
    public:
        ProductId(const Currency& quote_ccy, const Currency& base_ccy);

        const Currency& get_base_ccy() const {
            return base_ccy_;
        }

        const Currency& get_quote_ccy() const {
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

    class Subscription {
    public:
        Subscription(const std::list<ProductId>& product_ids);

        const std::list<ProductId>& get_product_ids() const;

        ~Subscription() = default;
    private:
        const std::list<ProductId>& product_ids_;
    };

    std::ostream& operator << (std::ostream& out, const Subscription& subscription);

    class MarketdataClient {
    public:
        MarketdataClient(const Subscription& subscription);

        void connect();

        void disconnect();

        ~MarketdataClient();
    private:
        ix::WebSocket *websocket;
    };
}

#endif //LIBCOINBASE_WSAPI_H
