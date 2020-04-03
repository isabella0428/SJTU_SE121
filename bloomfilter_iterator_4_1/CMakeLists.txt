cmake_minimum_required (VERSION 2.8)

project(SkipList)

include_directories(./include)

SET(CMAKE_SYSTEM_NAME Linux)

SET(CMAKE_CXX_COMPILER /usr/local/bin/g++-9)

set(CMAKE_CXX_FLAGS "-std=c++17")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g")

include_directories(./include)

set(SRCS src/kvstore.cc src/bloomfilter.cc src/MurmurHash3.cc)
add_executable(Test1 test/correctness.cc ${SRCS})
add_executable(Test2 test/persistence.cc ${SRCS})
add_executable(TestIterator test/testIterator.cc ${SRCS})
add_executable(TestPQ test/testPQ.cc)
