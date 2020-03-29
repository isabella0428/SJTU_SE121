cmake_minimum_required (VERSION 2.8)

project(SkipList)

include_directories(./include)

SET(CMAKE_BUILD_TYPE "Debug")

SET(CMAKE_SYSTEM_NAME Linux)

SET(CMAKE_CXX_COMPILER /usr/bin/g++-9)

set(CMAKE_CXX_FLAGS "-std=c++17")  

include_directories(./include)

set(SRCS src/kvstore.cc src/bloomfilter.cc src/MurmurHash3.cc)

add_executable(Test1 test/correctness.cc ${SRCS})
add_executable
add_executable(TestIterator test/testIterator.cc ${SRCS})
