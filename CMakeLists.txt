cmake_minimum_required(VERSION 3.14)
project(libcrypto-mktdata C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

include(conan.cmake)

conan_cmake_run(REQUIRES boost/1.70.0@conan/stable
                         fmt/5.3.0@bincrafters/stable
                         libuv/1.27.0@bincrafters/stable
                         rapidjson/1.1.0@bincrafters/stable
                         spdlog/1.3.1@bincrafters/stable
                         gtest/1.8.1@bincrafters/stable
        BASIC_SETUP BUILD missing)

find_package(IXWebSockets REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(include ${LIBIXWEBSOCKETS_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR})
set(LIBS ${LIBS} ${CONAN_LIBS} ${LIBIXWEBSOCKETS_LIBRARIES} ${OPENSSL_LIBRARIES} ${ZLIB_LIBRARIES})
message(${CONAN_LIBS_BOOST})
list(REMOVE_ITEM LIBS boost_unit_test_framework)

add_library(libcrypto-mktdata src/core.cpp include/cloudwall/crypto-mktdata/core.h
        src/coinbase.cpp include/cloudwall/crypto-mktdata/coinbase.h src/bitmex.cpp
        include/cloudwall/crypto-mktdata/bitmex.h src/bitstamp.cpp
        include/cloudwall/crypto-mktdata/bitstamp.h)

add_executable(libcrypto-mktdata_test test/core_test.cpp test/coinbase_test.cpp test/bitmex_test.cpp
        test/bitstamp_test.cpp)
target_link_libraries(libcrypto-mktdata_test libcrypto-mktdata ${LIBS})

