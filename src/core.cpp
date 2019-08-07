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
#include <sstream>

using namespace cloudwall::core::marketdata;

Currency::Currency(const std::string& ccy_code) : ccy_code_(ccy_code) {}

CurrencyPair::CurrencyPair(const Currency& base_ccy, const Currency& quote_ccy)
        : base_ccy_(base_ccy), quote_ccy_(quote_ccy) { }

Channel::Channel(const std::string &name, const std::experimental::optional<CurrencyPair> &ccy_pair)
        : name_(name), ccy_pair_(ccy_pair) { }

Subscription::Subscription(const std::list<Channel>& channels): channels_(channels) { }

RawFeedMessage::RawFeedMessage(const std::string &raw_json) : raw_json_(raw_json) {
}

RawFeedMessage::~RawFeedMessage() = default;
