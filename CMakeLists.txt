cmake_minimum_required(VERSION 3.0)

project(word2vec-cpp)

#set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES distance.cc)

add_executable(
    demo
    demo.cc
    words_model.h
    ordered_vector.h
    result_item.cc
    result_item.h
    result_set.cc
    result_set.h
)

target_link_libraries(
    demo
    pthread
)