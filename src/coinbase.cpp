#include <cloudwall/crypto-mktdata/coinbase.h>
#include <iostream>
#include <sstream>

using namespace cloudwall::coinbase::marketdata;
using cloudwall::core::marketdata::Currency;

ProductId::ProductId(const Currency& quote_ccy, const Currency& base_ccy)
    : base_ccy_(base_ccy), quote_ccy_(quote_ccy) { }

std::ostream& cloudwall::coinbase::marketdata::operator << (std::ostream& out, const ProductId& product_id) {
    out << product_id.get_quote_ccy() << "-" << product_id.get_base_ccy();
    return out;
}

Channel::Channel(const std::string &name, const std::list<ProductId> &product_ids)
    : name_(name), product_ids_(product_ids) { }

Subscription::Subscription(const std::list<Channel>& channels)
    : channels_(channels) { }

std::ostream& cloudwall::coinbase::marketdata::operator << (std::ostream& out, const Subscription& subscription) {
    rapidjson::Document d;
    rapidjson::Pointer("/type").Set(d, "subscribe");

    int i = 0;
    const std::list<Channel> &channels = subscription.get_channels();
    for (auto channel_iter = channels.begin(); channel_iter != channels.end(); i++, channel_iter++) {
        auto channel = *channel_iter;
        auto channel_json_ptr = fmt::format("/channels/{0}/name", i);
        rapidjson::Pointer(channel_json_ptr.c_str()).Set(d, channel.get_name().c_str());

        int j = 0;
        const std::list<ProductId> &product_ids = channel.get_product_ids();
        for (auto product_id_iter = product_ids.begin(); product_id_iter != product_ids.end(); j++, product_id_iter++) {
            auto product_id = *product_id_iter;
            auto id_json_ptr = fmt::format("/channels/{0}/product_ids/{1}", i, j);
            auto product_id_txt = fmt::format("{0}-{1}", product_id.get_quote_ccy().get_ccy_code(),
                    product_id.get_base_ccy().get_ccy_code());
            rapidjson::Pointer(id_json_ptr.c_str()).Set(d, product_id_txt.c_str());
        }
    }

    rapidjson::OStreamWrapper osw(out);
    rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
    d.Accept(writer);
    return out;
}

RawFeedMessage::RawFeedMessage(const std::string &raw_json) : raw_json_(raw_json) {
}

RawFeedMessage::~RawFeedMessage() = default;

RawFeedClient::RawFeedClient(const Subscription& subscription, const OnMessageCallback& callback)
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
                    // start subscription to heartbeat channel
                    std::stringstream ss;
                    ss << subscription;
                    spdlog::info("Connected to exchange; subscribing: {}", ss.str().c_str());
                    this->websocket_->send(ss.str());
                } else if (msg->type == ix::WebSocketMessageType::Close) {
                    spdlog::info("Connection closed");
                } else if (msg->type == ix::WebSocketMessageType::Message) {
                    SPDLOG_TRACE("Incoming message: {}", msg->str.c_str());
                    callback_(RawFeedMessage(msg->str));
                } else if (msg->type == ix::WebSocketMessageType::Error) {
                    std::stringstream ss;
                    ss << "Connection error: " << msg->errorInfo.reason      << std::endl;
                    ss << "#retries: "         << msg->errorInfo.retries     << std::endl;
                    ss << "Wait time(ms): "    << msg->errorInfo.wait_time   << std::endl;
                    ss << "HTTP Status: "      << msg->errorInfo.http_status << std::endl;
                    spdlog::info(ss.str());
                } else {
                    std::cout << "Unknown message type" << std::endl;
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

