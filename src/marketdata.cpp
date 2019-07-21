#include <marketdata.h>
#include <iostream>
#include <sstream>

coinbase::exchange::marketdata::Currency::Currency(const std::string& ccy_code) : ccy_code_(ccy_code) {}

std::ostream& coinbase::exchange::marketdata::operator << (std::ostream& out, const coinbase::exchange::marketdata::Currency& ccy) {
    out << ccy.get_ccy_code();
    return out;
}

coinbase::exchange::marketdata::ProductId::ProductId(const Currency& quote_ccy, const Currency& base_ccy)
    : base_ccy_(base_ccy), quote_ccy_(quote_ccy) { }

std::ostream& coinbase::exchange::marketdata::operator << (std::ostream& out, const coinbase::exchange::marketdata::ProductId& product_id) {
    out << product_id.get_quote_ccy() << "-" << product_id.get_base_ccy();
    return out;
}

coinbase::exchange::marketdata::Subscription::Subscription(const std::list<ProductId>& product_ids)
    : product_ids_(product_ids) { }

std::ostream& coinbase::exchange::marketdata::operator << (std::ostream& out, const coinbase::exchange::marketdata::Subscription& subscription) {
    nlohmann::json sub_json;
    sub_json["type"] = "subscribe";
    sub_json["channels"] = { "BTC-USD" };
    out << sub_json;
    return out;
}

coinbase::exchange::marketdata::MarketdataClient::MarketdataClient(const Subscription& subscription) {
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

void coinbase::exchange::marketdata::MarketdataClient::connect() {
    this->websocket->start();
}

void coinbase::exchange::marketdata::MarketdataClient::disconnect() {
    this->websocket->stop();
}


coinbase::exchange::marketdata::MarketdataClient::~MarketdataClient() {
    delete this->websocket;
}
