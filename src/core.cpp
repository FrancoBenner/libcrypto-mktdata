#include <cloudwall/crypto-mktdata/coinbase.h>
#include <iostream>
#include <sstream>

using namespace cloudwall::core::marketdata;

Currency::Currency(const std::string& ccy_code) : ccy_code_(ccy_code) {}

ProductId::ProductId(const Currency& quote_ccy, const Currency& base_ccy)
        : base_ccy_(base_ccy), quote_ccy_(quote_ccy) { }

RawFeedMessage::RawFeedMessage(const std::string &raw_json) : raw_json_(raw_json) {
}

RawFeedMessage::~RawFeedMessage() = default;
