cmake_minimum_required(VERSION 3.24)
project(DesignPatterns)

set(CMAKE_CXX_STANDARD 23)

add_executable(observer observer_main.cpp)

add_executable(crtp crtp_main.cpp)
add_executable(visitor visitor_main.cpp)
add_executable(strategy_main strategy_main.cpp)
add_executable(test_app test_app.cpp)
add_executable(type_erasure_app type_erasure_app.cpp)
add_executable(other_decorator_app other_decorator_app.cpp)