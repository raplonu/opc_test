cmake_minimum_required(VERSION 3.18)

project(opc_test VERSION 1.0.0 LANGUAGES CXX)

find_package(open62541 REQUIRED)
find_package(fmt REQUIRED)

add_library(opc_common INTERFACE)
target_include_directories(opc_common INTERFACE include)
target_compile_options(opc_common INTERFACE -Wno-write-strings)
target_link_libraries(opc_common INTERFACE open62541::open62541 fmt::fmt)

add_executable(opc_server server.cpp)
target_include_directories(opc_server PRIVATE include)
target_link_libraries(opc_server opc_common)


add_executable(opc_client1 client1.cpp)
target_include_directories(opc_client1 PRIVATE include)
target_link_libraries(opc_client1 opc_common)


add_executable(opc_client2 client2.cpp)
target_include_directories(opc_client2 PRIVATE include)
target_link_libraries(opc_client2 opc_common)
