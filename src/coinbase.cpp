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

RawFeedClient::RawFeedClient(const Subscription& subscription, const OnRawFeedMessageCallback& callback)
        : callback_(callback) {
    this->websocket_ = new ix::WebSocket();

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
                    spdlog::info("Connected to BitMEX exchange");

                    rapidjson::Document d;
                    rapidjson::Pointer("/type").Set(d, "subscribe");

                    int i = 0;
                    const std::list<Channel> &channels = subscription.get_channels();
                    for (auto channel_iter = channels.begin(); channel_iter != channels.end(); i++, channel_iter++) {
                        auto channel = *channel_iter;
                        auto channel_json_ptr = fmt::format("/channels/{0}/name", i);
                        rapidjson::Pointer(channel_json_ptr.c_str()).Set(d, channel.get_name().c_str());

                        int j = 0;
                        auto ccy_pair_opt = channel.get_ccy_pair();
                        if (ccy_pair_opt) {
                            auto ccy_pair = ccy_pair_opt.value();
                            auto id_json_ptr = fmt::format("/channels/{0}/product_ids/0", i, j);
                            auto ccy_pair_txt = fmt::format("{0}-{1}", ccy_pair.get_quote_ccy().get_ccy_code(),
                                                            ccy_pair.get_base_ccy().get_ccy_code());
                            rapidjson::Pointer(id_json_ptr.c_str()).Set(d, ccy_pair_txt.c_str());
                        }
                    }

                    std::stringstream ss;
                    rapidjson::OStreamWrapper osw(ss);
                    rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
                    d.Accept(writer);

                    spdlog::info("Subscribing to channel: {}", ss.str().c_str());
                    this->websocket_->send(ss.str());
                } else if (msg->type == ix::WebSocketMessageType::Close) {
                    spdlog::info("Connection to Coinbase Pro closed");
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

