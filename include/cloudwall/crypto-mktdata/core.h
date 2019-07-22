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

#ifndef CRYPTO_MKTDATA_CORE_H
#define CRYPTO_MKTDATA_CORE_H

#include <ostream>

namespace cloudwall::core::marketdata {
    class Currency {
    public:
        Currency(const std::string &ccy_code);

        const std::string &get_ccy_code() const {
            return ccy_code_;
        }

        bool operator==(const Currency &other) const {
            return (this->get_ccy_code() == other.get_ccy_code());
        }

        ~Currency() = default;

    private:
        const std::string &ccy_code_;
    };

    std::ostream &operator<<(std::ostream &out, const Currency &ccy);
}

#endif //CRYPTO_MKTDATA_CORE_H
