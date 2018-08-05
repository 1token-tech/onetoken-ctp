cmake_minimum_required(VERSION 2.8)

project(OneTokenCTP)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/build)
include (common)

add_definitions(-std=c++11 -DASIO_STANDALONE -D_WEBSOCKETPP_CPP11_STL_)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/common)

aux_source_directory(${PROJECT_SOURCE_DIR}/src DIR_SRCS)

link_libraries(ssl crypto z pthread)

add_library(otctp ${DIR_SRCS})
target_link_libraries(otctp PRIVATE "${PROJECT_SOURCE_DIR}/lib/libcpprest.so")
