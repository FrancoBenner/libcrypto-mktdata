cmake_minimum_required(VERSION 3.14)
project(coinbase C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

include(conan.cmake)

conan_cmake_run(REQUIRES fmt/5.3.0@bincrafters/stable
                         libuv/1.27.0@bincrafters/stable
                         jsonformoderncpp/3.6.1@vthiery/stable
                         gtest/1.8.1@bincrafters/stable
        BASIC_SETUP BUILD missing)

find_package(IXWebSockets REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(include ${LIBIXWEBSOCKETS_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR})
set(LIBS ${LIBS} ${CONAN_LIBS} ${LIBIXWEBSOCKETS_LIBRARIES} ${OPENSSL_LIBRARIES} ${ZLIB_LIBRARIES})

add_library(libcrypto-mktdata src/core.cpp include/cloudwall/crypto-mktdata/core.h
        src/coinbase.cpp include/cloudwall/crypto-mktdata/coinbase.h)

add_executable(libcrypto-mktdata_test test/coinbase_test.cpp)
target_link_libraries(libcrypto-mktdata_test libcrypto-mktdata ${LIBS})

