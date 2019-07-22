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

Subscription::Subscription(const std::list<ProductId>& product_ids)
    : product_ids_(product_ids) { }

std::ostream& cloudwall::coinbase::marketdata::operator << (std::ostream& out, const Subscription& subscription) {
    nlohmann::json sub_json;
    sub_json["type"] = "subscribe";
    sub_json["channels"] = { "BTC-USD" };
    out << sub_json;
    return out;
}

MarketdataClient::MarketdataClient(const Subscription& subscription) {
    std::cout << subscription << std::endl;

    this->websocket = new ix::WebSocket();

    std::string url("wss://ws-feed.pro.coinbase.com/");
    websocket->setUrl(url);

    // Optional heart beat, sent every 45 seconds when there is not any traffic
    // to make sure that load balancers do not kill an idle connection.
    websocket->setHeartBeatPeriod(45);

    // Setup a callback to be fired when a message or an event (open, close, error) is received
    websocket->setOnMessageCallback(
            [this](const ix::WebSocketMessagePtr& msg)
            {
                if (msg->type == ix::WebSocketMessageType::Open) {
                    // start subscription to heartbeat channel
                    this->websocket->send(
                            R"({"type": "subscribe", "channels": [ { "name": "matches", "product_ids": ["BTC-USD"] }, { "name": "heartbeat", "product_ids": ["BTC-USD"] }, { "name": "level2", "product_ids": ["BTC-USD"] }, { "name": "full", "product_ids": ["BTC-USD"] }, {"name": "status" } ] })");
                } else if (msg->type == ix::WebSocketMessageType::Close) {
                    std::cout << "connection closed" << std::endl;
                } else if (msg->type == ix::WebSocketMessageType::Message) {
                    std::cout << msg->str << std::endl;
                } else if (msg->type == ix::WebSocketMessageType::Error) {
                    std::stringstream ss;
                    ss << "Connection error: " << msg->errorInfo.reason      << std::endl;
                    ss << "#retries: "         << msg->errorInfo.retries     << std::endl;
                    ss << "Wait time(ms): "    << msg->errorInfo.wait_time   << std::endl;
                    ss << "HTTP Status: "      << msg->errorInfo.http_status << std::endl;
                    std::cout << ss.str() << std::endl;
                } else {
                    std::cout << "Unknown message type" << std::endl;
                }
            });
}

void MarketdataClient::connect() {
    this->websocket->start();
}

void MarketdataClient::disconnect() {
    this->websocket->stop();
}


MarketdataClient::~MarketdataClient() {
    delete this->websocket;
}
