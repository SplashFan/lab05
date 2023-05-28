[![Coverage Status](https://coveralls.io/repos/github/SplashFan/lab05/badge.svg?branch=refs/heads/master)](https://coveralls.io/github/SplashFan/lab05?branch=refs/heads/master)

## Laboratory work V

Данная лабораторная работа посвещена изучению фреймворков для тестирования на примере **GTest**

```sh
$ open https://github.com/google/googletest
```

## Homework

### Задание
1. Создайте `CMakeList.txt` для библиотеки *banking*.
2. Создайте модульные тесты на классы `Transaction` и `Account`.
    * Используйте mock-объекты.
    * Покрытие кода должно составлять 100%.
3. Настройте сборочную процедуру на **TravisCI**.
4. Настройте [Coveralls.io](https://coveralls.io/).

```
$ cd SplashFan/workspace/projects
~/SplashFan/workspace/projects$ git clone https://github.com/tp-labs/lab05 lab05
Cloning into 'lab05'...
remote: Enumerating objects: 137, done.
remote: Counting objects: 100% (25/25), done.
remote: Compressing objects: 100% (9/9), done.
remote: Total 137 (delta 18), reused 16 (delta 16), pack-reused 112
Receiving objects: 100% (137/137), 918.92 KiB | 1.84 MiB/s, done.
Resolving deltas: 100% (60/60), done.
```
```
~/SplashFan/workspace/projects$ cd lab05
~/SplashFan/workspace/projects/lab05$ git remote remove origin
~/SplashFan/workspace/projects/lab05$ git remote add origin https://github.com/SplashFan/lab05
```
```
~/SplashFan/workspace/projects/lab05$ git submodule add  https://github.com/google/googletest.git
Cloning into '/home/splashfan/SplashFan/workspace/projects/lab05/googletest'...
remote: Enumerating objects: 26376, done.
remote: Counting objects: 100% (274/274), done.
remote: Compressing objects: 100% (152/152), done.
remote: Total 26376 (delta 145), reused 191 (delta 110), pack-reused 26102
Receiving objects: 100% (26376/26376), 12.48 MiB | 2.62 MiB/s, done.
Resolving deltas: 100% (19466/19466), done.
```
```
~/SplashFan/workspace/projects/lab05$ cd banking
~/SplashFan/workspace/projects/lab05/banking$ rm CMakeList.txt
~/SplashFan/workspace/projects/lab05/banking$ nano CMakeLists.txt
```
```
cmake_minimum_required(VERSION 3.4)
project(bank_lib)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(banking STATIC Account.cpp Account.h Transaction.cpp Transaction.h)
```
```
~/SplashFan/workspace/projects/lab05$ mkdir .github
~/SplashFan/workspace/projects/lab05$ mkdir .github/workflows
~/SplashFan/workspace/projects/lab05$ cd .github/workflows
~/SplashFan/workspace/projects/lab05/.github/workflows$ nano cmake.yml
```
```
name: CMake
on:
 push:
  branches: [master]
 pull_request:
  branches: [master]
jobs:
 build_Linux:
  runs-on: ubuntu-latest
  steps:
  - uses: actions/checkout@v3
  - name: Adding gtest
    run: git clone https://github.com/google/googletest.git third-party/gtest -b release-1.11.0
  - name: Install lcov
    run: sudo apt-get install -y lcov
  - name: Config banking with tests
    run: cmake -H. -B ${{github.workspace}}/build -DBUILD_TESTS=ON
  - name: Build banking
    run: cmake --build ${{github.workspace}}/build
  - name: Run tests
    run: build/check
  - name: Do lcov stuff
    run: lcov -c -d build/CMakeFiles/banking.dir/banking/ --include *.cpp --output-file ./coverage/lcov.info
  - name: Publish to coveralls.io
    uses: coverallsapp/github-action@v1.1.2
    with:
      github-token: ${{ secrets.GITHUB_TOKEN }}
      ```
      
      ~/SplashFan/workspace/projects/lab05$ nano CMakeLists.txt
      ```
      cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS)
  add_compile_options(--coverage)
endif()

project (banking)

add_library(banking STATIC ${CMAKE_CURRENT_SOURCE_DIR}/banking/Transaction.cpp ${CMAKE_CURRENT_SOURCE_DIR}/banking/Account.cpp)
target_include_directories(banking PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/banking )

target_link_libraries(banking gcov)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(third-party/gtest)
  file(GLOB BANKING_TEST_SOURCES tests/*.cpp)
  add_executable(check ${BANKING_TEST_SOURCES})
  target_link_libraries(check banking gtest_main)
  add_test(NAME check COMMAND check)
endif()
```
```
~/SplashFan/workspace/projects/lab05$ mkdir tests
~/SplashFan/workspace/projects/lab05$ cd tests
~/SplashFan/workspace/projects/lab05/tests$ nano test_Account.cpp
```
```
#include <Account.h>
#include <gtest/gtest.h>

TEST(Account, Banking){
	Account test(0,0);
	ASSERT_EQ(test.GetBalance(), 0);
	ASSERT_THROW(test.ChangeBalance(100), std::runtime_error);
	test.Lock();
	ASSERT_NO_THROW(test.ChangeBalance(100));
	ASSERT_EQ(test.GetBalance(), 100);
	ASSERT_THROW(test.Lock(), std::runtime_error);
	test.Unlock();
	ASSERT_THROW(test.ChangeBalance(100), std::runtime_error);
}
```

~/SplashFan/workspace/projects/lab05/tests$ nano test_Transaction.cpp
```
#include <Account.h>
#include <Transaction.h>
#include <gtest/gtest.h>

TEST(Transaction, Banking){
	const int base_A = 5000, base_B = 5000, base_fee = 100;
	Account Alice(0,base_A), Bob(1,base_B);
	Transaction test_tran;
	ASSERT_EQ(test_tran.fee(), 1);
	test_tran.set_fee(base_fee);
	ASSERT_EQ(test_tran.fee(), base_fee);
	ASSERT_THROW(test_tran.Make(Alice, Alice, 1000), std::logic_error);
	ASSERT_THROW(test_tran.Make(Alice, Bob, -50), std::invalid_argument);
	ASSERT_THROW(test_tran.Make(Alice, Bob, 50), std::logic_error);
	if (test_tran.fee()*2-1 >= 100)
		ASSERT_EQ(test_tran.Make(Alice, Bob, test_tran.fee()*2-1), false);
	Alice.Lock();
	ASSERT_THROW(test_tran.Make(Alice, Bob, 1000), std::runtime_error);
	Alice.Unlock();
	ASSERT_EQ(test_tran.Make(Alice, Bob, 1000), true);
	ASSERT_EQ(Bob.GetBalance(), base_B+1000);
	ASSERT_EQ(Alice.GetBalance(), base_A-1000-base_fee);
	ASSERT_EQ(test_tran.Make(Alice, Bob, 3900), false);
	ASSERT_EQ(Bob.GetBalance(), base_B+1000);
	ASSERT_EQ(Alice.GetBalance(), base_A-1000-base_fee);
}
```

```
~/SplashFan/workspace/projects/lab05$ mkdir coverage
~/SplashFan/workspace/projects/lab05$ cd coverage
~/SplashFan/workspace/projects/lab05/coverage$ touch temp.txt
~/SplashFan/workspace/projects/lab05$ git add -A
~/SplashFan/workspace/projects/lab05$ git commit -m"from tp-labs with tests"
[master 20b13a7] from tp-labs with tests
 10 files changed, 104 insertions(+), 2 deletions(-)
 create mode 100644 .github/workflows/cmake.yml
 create mode 100644 .gitmodules
 create mode 100644 CMakeLists.txt
 delete mode 100644 banking/CMakeList.txt
 create mode 100644 banking/CMakeLists.txt
 create mode 100644 coverage/temp.txt
 create mode 160000 googletest
 create mode 100644 tests/test_Account.cpp
 create mode 100644 tests/test_Transaction.cpp
```
```
~/SplashFan/workspace/projects/lab05$ git push origin master
Username for 'https://github.com': SplashFan
Password for 'https://SplashFan@github.com': 
Enumerating objects: 152, done.
Counting objects: 100% (152/152), done.
Delta compression using up to 3 threads
Compressing objects: 100% (85/85), done.
Writing objects: 100% (152/152), 921.34 KiB | 230.33 MiB/s, done.
Total 152 (delta 62), reused 135 (delta 60), pack-reused 0
remote: Resolving deltas: 100% (62/62), done.
To https://github.com/SplashFan/lab05
 * [new branch]      master -> master
```


## Links

- [C++ CI: Travis, CMake, GTest, Coveralls & Appveyor](http://david-grs.github.io/cpp-clang-travis-cmake-gtest-coveralls-appveyor/)
- [Boost.Tests](http://www.boost.org/doc/libs/1_63_0/libs/test/doc/html/)
- [Catch](https://github.com/catchorg/Catch2)

```
Copyright (c) 2015-2021 The ISC Authors
```
