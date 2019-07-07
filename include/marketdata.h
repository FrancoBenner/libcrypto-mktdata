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

#ifndef LIBCOINBASE_WSAPI_H
#define LIBCOINBASE_WSAPI_H

#include <libwebsockets.h>

namespace coinbase {
    namespace exchange {
        namespace marketdata {
            class MarketdataClient {

                void connect();
                void disconnect();
            };
        }
    }
}

#endif //LIBCOINBASE_WSAPI_H