cmake_minimum_required(VERSION 3.21)

project(Sol
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ADD_TEST_SOL "If test should be built" OFF)

add_subdirectory(app)

if(ADD_TEST_SOL)
    enable_testing()
    add_subdirectory(test)
else()
    add_subdirectory(example)
endif()
