# 设置最低的 CMake 版本要求
cmake_minimum_required(VERSION 3.10)

# 定义项目名称
project(muduo_test)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 设置详细的构建输出
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Wno-unused-function")

set(LIB_SRC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/InetAddress.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Timestamp.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Logger.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/EventLoop.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Channel.cc
)

add_library(muduo_test SHARED ${LIB_SRC})
target_include_directories(muduo_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(test ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_Timestamp.cc)
add_dependencies(test muduo_test)
target_link_libraries(test muduo_test)


SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)