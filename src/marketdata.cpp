#include <marketdata.h>
#include <iostream>

coinbase::exchange::marketdata::MarketdataClient::MarketdataClient() {
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
