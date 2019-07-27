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

#include <iostream>
#include <sstream>

#include <boost/algorithm/string.hpp>

#include <cloudwall/crypto-mktdata/bitstamp.h>

using namespace cloudwall::bitstamp::marketdata;

std::ostream& cloudwall::bitstamp::marketdata::operator << (std::ostream& out, const ProductId& product_id) {
    out << product_id.get_quote_ccy() << product_id.get_base_ccy();
    return out;
}

std::ostream& cloudwall::bitstamp::marketdata::operator << (std::ostream& out, const Currency& ccy) {
    out << boost::algorithm::to_lower_copy(ccy.get_ccy_code());
    return out;
}

Channel::Channel(const std::string &name, const std::experimental::optional<ProductId> &product_id)
        : name_(name), product_id_(product_id) { }

Subscription::Subscription(const std::list<Channel>& channels): channels_(channels) { }

RawFeedClient::RawFeedClient(const Subscription& subscription, const OnRawFeedMessageCallback& callback)
        : callback_(callback) {
    this->websocket_ = new ix::WebSocket();

    std::string url("wss://ws.bitstamp.net/");
    websocket_->setUrl(url);

    // Optional heart beat, sent every 45 seconds when there is not any traffic
    // to make sure that load balancers do not kill an idle connection.
    websocket_->setHeartBeatPeriod(45);

    // Setup a callback to be fired when a message or an event (open, close, error) is received
    websocket_->setOnMessageCallback(
            [this, subscription](const ix::WebSocketMessagePtr& msg)
            {
                if (msg->type == ix::WebSocketMessageType::Open) {
                    int i = 0;
                    const std::list<Channel> &channels = subscription.get_channels();
                    for (auto channel_iter = channels.begin(); channel_iter != channels.end(); i++, channel_iter++) {
                        std::stringstream ss;
                        rapidjson::OStreamWrapper osw(ss);

                        rapidjson::Document d;
                        rapidjson::Pointer("/event").Set(d, "bts:subscribe");

                        auto channel = (*channel_iter).get_name();
                        auto product_id_opt = (*channel_iter).get_product_id();
                        auto arg_json_ptr = "/data/channel";

                        if (product_id_opt) {
                            auto product_id = product_id_opt.value();
                            auto quote_ccy = boost::algorithm::to_lower_copy(product_id.get_quote_ccy().get_ccy_code());
                            auto base_ccy = boost::algorithm::to_lower_copy(product_id.get_base_ccy().get_ccy_code());
                            auto product_id_txt = fmt::format("{0}_{1}{2}", channel, quote_ccy, base_ccy);
                            rapidjson::Pointer(arg_json_ptr).Set(d, product_id_txt.c_str());
                        } else {
                            rapidjson::Pointer(arg_json_ptr).Set(d, channel.c_str());
                        }

                        rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
                        d.Accept(writer);

                        spdlog::info("Subscribing to channel: {}", ss.str().c_str());
                        this->websocket_->send(ss.str());
                    }
                } else if (msg->type == ix::WebSocketMessageType::Close) {
                    spdlog::info("Connection to Bitstamp closed");
                } else if (msg->type == ix::WebSocketMessageType::Message) {
                    SPDLOG_TRACE("Incoming message: {}", msg->str.c_str());
                    callback_(RawFeedMessage(msg->str));
                } else if (msg->type == ix::WebSocketMessageType::Error) {
                    std::stringstream ss;
                    ss << "Connection error: " << msg->errorInfo.reason << std::endl;
                    ss << "# retries: " << msg->errorInfo.retries << std::endl;
                    ss << "Wait time (ms): " << msg->errorInfo.wait_time << std::endl;
                    ss << "HTTP Status: " << msg->errorInfo.http_status << std::endl;
                    spdlog::info(ss.str());
                } else {
                    spdlog::error("Unknown message type");
                }
            });
}

void RawFeedClient::connect() {
    this->websocket_->start();
}

void RawFeedClient::disconnect() {
    this->websocket_->stop();
}

RawFeedClient::~RawFeedClient() {
    delete this->websocket_;
}
