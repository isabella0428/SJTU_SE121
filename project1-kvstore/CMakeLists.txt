cmake_minimum_required (VERSION 2.8)

project(SkipList)

include_directories(./include)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_SYSTEM_NAME Linux)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g")

set(CMAKE_CXX_FLAGS "-std=c++17")

set(SRCS src/kvstore.cc src/bloomfilter.cc src/MurmurHash3.cc)
add_executable(correctnessTest test/correctness.cc ${SRCS})
add_executable(persistenceTest test/persistence.cc ${SRCS})
add_executable(speedTest test/testSpeed.cc ${SRCS})
add_executable(throughputTest test/testThroughput.cc ${SRCS})
