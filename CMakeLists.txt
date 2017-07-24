cmake_minimum_required(VERSION 2.8.9)
project (samsung_test_task)
set (CMAKE_CXX_STANDARD 11)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_subdirectory(samsung_test_task)
add_subdirectory(vigenere_cipher)
 
