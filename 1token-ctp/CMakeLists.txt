cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)

project (OnetokenCTP)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/build)
include (common)

add_shared_library ( onetoken_ctp
)