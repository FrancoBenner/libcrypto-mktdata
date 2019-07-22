#include <cloudwall/crypto-mktdata/coinbase.h>
#include <iostream>
#include <sstream>

using namespace cloudwall::core::marketdata;

Currency::Currency(const std::string& ccy_code) : ccy_code_(ccy_code) {}

std::ostream& cloudwall::core::marketdata::operator << (std::ostream& out, const Currency& ccy) {
    out << ccy.get_ccy_code();
    return out;
}