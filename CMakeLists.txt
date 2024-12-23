# filepath: /c/workdir/helloWorld/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(HelloWorld)

# Add an executable
add_executable(hello hello.cpp)

# Find Google Benchmark
find_package(benchmark REQUIRED)
target_link_libraries(hello benchmark::benchmark)