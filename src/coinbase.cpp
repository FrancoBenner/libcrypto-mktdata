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

#include <cloudwall/crypto-mktdata/coinbase.h>
#include <iostream>
#include <sstream>

using namespace cloudwall::coinbase::marketdata;

using cloudwall::core::marketdata::Channel;
using cloudwall::core::marketdata::RawFeedMessage;

CoinbaseRawFeedClient::CoinbaseRawFeedClient(const Subscription& subscription,
        const OnRawFeedMessageCallback& callback) : RawFeedClient(new ix::WebSocket(), callback) {
    std::string url("wss://ws-feed.pro.coinbase.com/");
    websocket_->setUrl(url);

    // Optional heart beat, sent every 45 seconds when there is not any traffic
    // to make sure that load balancers do not kill an idle connection.
    websocket_->setHeartBeatPeriod(45);

    // Setup a callback to be fired when a message or an event (open, close, error) is received
    websocket_->setOnMessageCallback(
            [this, subscription](const ix::WebSocketMessagePtr& msg)
            {
                if (msg->type == ix::WebSocketMessageType::Open) {
                    spdlog::info("Connected to Coinbase Pro exchange");

                    const std::list<Channel> &channels = subscription.get_channels();
                    for (auto channel : channels) {
                        rapidjson::Document d;
                        rapidjson::Pointer("/type").Set(d, "subscribe");

                        auto channel_json_ptr = "/channels/0/name";
                        rapidjson::Pointer(channel_json_ptr).Set(d, channel.get_name().c_str());

                        auto ccy_pair_opt = channel.get_ccy_pair();
                        if (ccy_pair_opt) {
                            auto ccy_pair = ccy_pair_opt.value();
                            auto id_json_ptr = "/channels/0/product_ids/0";
                            auto ccy_pair_txt = fmt::format("{0}-{1}", ccy_pair.get_base_ccy().get_ccy_code(),
                                                            ccy_pair.get_quote_ccy().get_ccy_code());
                            rapidjson::Pointer(id_json_ptr).Set(d, ccy_pair_txt.c_str());
                        }

                        std::stringstream ss;
                        rapidjson::OStreamWrapper osw(ss);
                        rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
                        d.Accept(writer);

                        spdlog::info("Subscribing to channel: {}", ss.str().c_str());
                        this->websocket_->send(ss.str());
                    }
                } else if (msg->type == ix::WebSocketMessageType::Close) {
                    spdlog::info("Connection to Coinbase Pro closed");
                } else if (msg->type == ix::WebSocketMessageType::Message) {
                    SPDLOG_TRACE("Incoming message from Coinbase Pro: {}", msg->str.c_str());
                    callback_(RawFeedMessage(msg->str));
                } else if (msg->type == ix::WebSocketMessageType::Error) {
                    std::stringstream ss;
                    ss << "Connection error: " << msg->errorInfo.reason << std::endl;
                    ss << "# retries: " << msg->errorInfo.retries << std::endl;
                    ss << "Wait time (ms): " << msg->errorInfo.wait_time << std::endl;
                    ss << "HTTP Status: " << msg->errorInfo.http_status << std::endl;
                    spdlog::info(ss.str());
                } else if (msg->type == ix::WebSocketMessageType::Pong) {
                     spdlog::debug("received pong message");
                } else {
                    spdlog::error("Unknown message type");
                }
            });
}

CoinbaseEventClient::CoinbaseEventClient(const Subscription& subscription, const OnCoinbaseEventCallback& callback) {
    OnRawFeedMessageCallback raw_callback = [callback](const RawFeedMessage& message) {
        auto d = rapidjson::Document();
        auto raw_json = message.get_raw_json();
        d.Parse(raw_json.c_str());
        if (d["type"] == "status") {
            callback(ProductStatusEvent(d));
        }
    };
    this->raw_feed_client_ = new CoinbaseRawFeedClient(subscription, raw_callback);
}

ProductStatus::ProductStatus(rapidjson::Value::ConstValueIterator product_json_iter) {
    auto product_json = product_json_iter->GetObject();

    std::string base_ccy_id = product_json["base_currency"].GetString();
    std::string quote_ccy_id = product_json["quote_currency"].GetString();
    auto base_ccy = Currency(base_ccy_id);
    auto quote_ccy = Currency(quote_ccy_id);

    this->id_ = new std::string(product_json["id"].GetString());
    this->ccy_pair_ = new CurrencyPair(base_ccy, quote_ccy);
    this->status_ = new std::string(product_json["status"].GetString());
    this->status_message_ = new std::string(product_json["status_message"].GetString());
}

ProductStatus::~ProductStatus() {
    delete id_;
    delete ccy_pair_;
    delete status_;
    delete status_message_;
}

ProductStatusEvent::ProductStatusEvent(const std::string& raw_json) {
    auto d = rapidjson::Document();
    d.Parse(raw_json.c_str());
    init(d);
}

ProductStatusEvent::ProductStatusEvent(const rapidjson::Document& doc) {
    init(doc);
}

void ProductStatusEvent::init(const rapidjson::Document& doc) {
    this->products_ = new std::list<ProductStatus*>();
    const rapidjson::Value& products = doc["products"];
    for (rapidjson::Value::ConstValueIterator itr = products.Begin(); itr != products.End(); ++itr) {
        this->products_->emplace_back(new ProductStatus(itr));
    }
}

ProductStatusEvent::~ProductStatusEvent() {
    delete this->products_;
}

