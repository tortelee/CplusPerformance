# filepath: /c/workdir/helloWorld/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(HelloWorld)

# Add an executable
add_executable(hello hello.cpp)

# Collect all .cpp files in the project directory
#file(GLOB_RECURSE ALL_CPP_FILES "${CMAKE_SOURCE_DIR}/*.cpp")

# Add the executable
#add_executable(cplusperformance ${ALL_CPP_FILES})

# Find Google Benchmark
find_package(benchmark REQUIRED)
target_link_libraries(hello benchmark::benchmark)