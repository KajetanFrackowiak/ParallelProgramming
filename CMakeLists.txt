cmake_minimum_required(VERSION 3.0)
project(YourProjectName)

# Add your executable
add_executable(your_executable main.cpp)

# Include TBB headers
target_include_directories(your_executable PRIVATE /opt/intel/oneapi/tbb/latest/include)

# Link TBB library
target_link_libraries(your_executable PRIVATE /opt/intel/oneapi/tbb/latest/lib/libtbb.so)

set(CMAKE_CXX_COMPILER "dpcpp")

target_compile_options(your_executable PRIVATE -fsycl)