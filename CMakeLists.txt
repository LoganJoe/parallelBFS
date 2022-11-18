cmake_minimum_required(VERSION 3.12)

project(BFS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_C_FLAGS "-O3")

find_package(OpenMP)

file(GLOB SRC_FILES src/*.cpp)

add_executable(main main.cpp ${SRC_FILES} ${INTERNAL_SRC_FILES} )
target_include_directories(main PRIVATE include)
target_compile_features(main PRIVATE cxx_std_17)
target_compile_definitions(main PRIVATE ${DEFINES})
target_link_libraries(main OpenMP::OpenMP_CXX)