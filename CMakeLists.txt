cmake_minimum_required(VERSION 3.0)

project(word2vec-cpp)

#set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(.)

add_library(
    word2vec-parser
    lib/ordered_vector.h
    lib/result_item.cc
    lib/result_set.cc
    lib/words_model.cc
)

add_executable(
    demo
    src/main.cc
)

target_link_libraries(
    demo
    word2vec-parser
    pthread
)