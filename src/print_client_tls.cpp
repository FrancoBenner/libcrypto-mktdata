#include <cloudwall/crypto-mktdata/websocket_client.h>

int main(int argc, char* argv[]) {
    auto hostname = new std::string("ws-feed.pro.coinbase.com");
    auto uri = new std::string("wss://" + *hostname);

    auto ioc = new boost::asio::io_context();
    auto ssl_ctx = new cloudwall::websocket::SSLContext(hostname);
    auto ws = cloudwall::websocket::Websocket(uri, ioc, ssl_ctx);
    ws.set_on_open_callback([](cloudwall::websocket::Websocket* ws) {
        ws->send("{\n"
                 "    \"type\": \"subscribe\",\n"
                 "    \"product_ids\": [\n"
                 "        \"BTC-USD\"\n"
                 "    ],\n"
                 "    \"channels\": [\n"
                 "        \"matches\",\n"
                 "        \"heartbeat\",\n"
                 "        {\n"
                 "            \"name\": \"ticker\",\n"
                 "            \"product_ids\": [\n"
                 "                \"BTC-USD\"\n"
                 "            ]\n"
                 "        }\n"
                 "    ]\n"
                 "}");
    });

    ws.set_on_message_callback([](const cloudwall::websocket::Websocket* ws,
            const cloudwall::websocket::WebsocketMessage *msg) {
        std::cout << msg->get_payload() << std::endl;
    });
    ws.set_on_close_callback([](const cloudwall::websocket::Websocket* ws) {
        std::cout << "closing ..." << std::endl;
    });

    ws.connect();
    ioc->run();
}