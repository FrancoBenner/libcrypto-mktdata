cmake_minimum_required(VERSION 3.14)
project(coinbase C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_run(REQUIRES fmt/5.3.0@bincrafters/stable
                         libuv/1.27.0@bincrafters/stable
                         jsonformoderncpp/3.6.1@vthiery/stable
                         rapidjson/1.1.0@bincrafters/stable
                         gtest/1.8.1@bincrafters/stable
        BASIC_SETUP BUILD missing)

find_package(LibWebSockets REQUIRED)

include_directories(include ${LIBWEBSOCKETS_INCLUDE_DIR})
set(LIBS ${LIBS} ${CONAN_LIBS} ${LIBWEBSOCKETS_INCLUDE_LIBRARIES})

add_library(coinbase src/config.cpp src/marketdata.cpp)
target_link_libraries(coinbase ${LIBS})

add_executable(coinbase_test test/config_test.cpp test/marketdata_test.cpp)
target_link_libraries(coinbase_test coinbase ${LIBS})

