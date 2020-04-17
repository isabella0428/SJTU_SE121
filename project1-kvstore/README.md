## Project 1: KVStore using Log-structured Merge Tree


The handout files include two main parts:

- The `KVStoreAPI` class in `kvstore_api.h` that specifies the interface of KVStore.
- Test files including correctness test (`correctness.cc`) and persistence test (`persistence.cc`).

Explanation of each handout file:

```text
.
├── Makefile  // Makefile if you use GNU Make
├── README.md // This readme file
├── correctness.cc // Correctness test, you should not modify this file
├── data      // Data directory used in our test
├── kvstore.cc     // your implementation
├── kvstore.h      // your implementation
├── kvstore_api.h  // KVStoreAPI, you should not modify this file
├── persistence.cc // Persistence test, you should not modify this file
└── test.h         // Base class for testing, you should not modify this file
```


First have a look at the `kvstore_api.h` file to check functions you need to implement. Then modify the `kvstore.cc` and `kvstore.h` files and feel free to add new class files.

We will use all files with `.cc`, `.cpp`, `.cxx` suffixes to build correctness and persistence tests. Thus, you can use any IDE to finish this project as long as you ensure that all C++ source files are submitted.

For the test files, of course you could modify it to debug your programs. But remember to change it back when you are testing.



#### Test

1. ##### correctnessTest

```
1 cd ./project1_kvstore
2 cmake -D cmake -D CMAKE_CXX_COMPILER=""(Your gcc path, gcc version>=9)
3 make
4 ./correctnessTest
```



2. ##### persistenceTest

Persistence data preparation

```
1 cd ./project1_kvstore
2 cmake -D cmake -D CMAKE_CXX_COMPILER=""(Your gcc path, gcc version>=9)
3 make
4 ./persistenceTest
```

Persistence Test

```
1 cd ./project1_kvstore
2 cmake -D cmake -D CMAKE_CXX_COMPILER=""(Your gcc path, gcc version>=9)
3 make
4 ./persistenceTest -t
```





