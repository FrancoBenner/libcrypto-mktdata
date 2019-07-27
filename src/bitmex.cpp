#include <cloudwall/crypto-mktdata/bitmex.h>
#include <iostream>
#include <sstream>

using namespace cloudwall::bitmex::marketdata;

std::ostream& cloudwall::bitmex::marketdata::operator << (std::ostream& out, const ProductId& product_id) {
    out << product_id.get_quote_ccy() << product_id.get_base_ccy();
    return out;
}

std::ostream& cloudwall::bitmex::marketdata::operator << (std::ostream& out, const Currency& ccy) {
    out << ccy.get_ccy_code();
    return out;
}

Topic::Topic(const std::string &name, const std::experimental::optional<ProductId> &product_id)
    : name_(name), product_id_(product_id) { }

Subscription::Subscription(const std::list<Topic>& topics): topics_(topics) { }

std::ostream& cloudwall::bitmex::marketdata::operator << (std::ostream& out, const Subscription& subscription) {
    rapidjson::Document d;
    rapidjson::Pointer("/op").Set(d, "subscribe");

    int i = 0;
    const std::list<Topic> &topics = subscription.get_topics();
    for (auto topic_iter = topics.begin(); topic_iter != topics.end(); i++, topic_iter++) {
        auto topic = (*topic_iter).get_name();
        auto product_id_opt = (*topic_iter).get_product_id();
        auto arg_json_ptr = fmt::format("/args/{0}", i);

        if (product_id_opt) {
            auto product_id = product_id_opt.value();
            auto product_id_txt = fmt::format("{0}:{1}{2}", topic, product_id.get_quote_ccy().get_ccy_code(),
                                              product_id.get_base_ccy().get_ccy_code());
            rapidjson::Pointer(arg_json_ptr.c_str()).Set(d, product_id_txt.c_str());
        } else {
            rapidjson::Pointer(arg_json_ptr.c_str()).Set(d, topic.c_str());
        }
    }

    rapidjson::OStreamWrapper osw(out);
    rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
    d.Accept(writer);
    return out;
}

RawFeedClient::RawFeedClient(const Subscription& subscription, const OnRawFeedMessageCallback& callback)
        : callback_(callback) {
    this->websocket_ = new ix::WebSocket();

    std::string url("wss://www.bitmex.com/realtime/");
    websocket_->setUrl(url);

    // Optional heart beat, sent every 45 seconds when there is not any traffic
    // to make sure that load balancers do not kill an idle connection.
    websocket_->setHeartBeatPeriod(45);

    // Setup a callback to be fired when a message or an event (open, close, error) is received
    websocket_->setOnMessageCallback(
            [this, subscription](const ix::WebSocketMessagePtr& msg)
            {
                if (msg->type == ix::WebSocketMessageType::Open) {
                    // start subscription to heartbeat topic
                    std::stringstream ss;
                    ss << subscription;
                    spdlog::info("Connected to BitMEX; subscribing: {}", ss.str().c_str());
                    this->websocket_->send(ss.str());
                } else if (msg->type == ix::WebSocketMessageType::Close) {
                    spdlog::info("Connection to BitMEX closed");
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
