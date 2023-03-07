cmake_minimum_required(VERSION 3.20)

project(AnyaSTL)

# C++标准
set(CMAKE_CXX_STANDARD 20)

# gtest版本
set(GOOGLETEST_VERSION 1.13.0)

# 添加子模块
add_subdirectory(./external/googlemock)

# 头文件目录
include_directories(${PROJECT_SOURCE_DIR}/src/core ./external/googletest/include ./external/googlemock/include)

# 第三方库目录
link_directories(${PROJECT_SOURCE_DIR}/external/googletest ${PROJECT_SOURCE_DIR}/external/googlemock)

# 创建目标
add_executable(main)

# 递归搜索文件并自动更新
file(GLOB_RECURSE source CONFIGURE_DEPENDS src/*.cpp src/*.c src/*.hpp src/tests/*.cpp)

# 添加源文件
target_sources(main PRIVATE ${source})

# 链接第三方库
target_link_libraries(main gtest gmock)

# 使用Release版本
# SET(CMAKE_BUILD_TYPE "Release")