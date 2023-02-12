cmake_minimum_required(VERSION 3.21)

message(STATUS "Starting project setup....")

project(Sol
    LANGUAGES CXX
)

set(CPP_STANDARD 20)

set(CMAKE_CXX_STANDARD ${CPP_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "Platform : ${PLATFORM}")
message(STATUS "CPP Standard : ${CPP_STANDARD}")

if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
endif()

file(GLOB_RECURSE SRC src/*.cpp includes/*.hpp exports/*.hpp templates/*.hpp)

if(PLATFORM STREQUAL "WINDOWS")
    add_executable(${PROJECT_NAME} WIN32
        ${SRC}
        icon/win32ExecutableIcon.rc
    )
else()
    add_executable(${PROJECT_NAME}
        ${SRC}
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE includes/ exports/ templates/ templates/modules/ ThirdPartyLibraries/DirectXMath/Inc/ ThirdPartyLibraries/DirectXMath/Extensions/ ThirdPartyLibraries/stb/)

target_compile_definitions(${PROJECT_NAME} PRIVATE "$<$<CONFIG:DEBUG>:_DEBUG>" "$<$<CONFIG:RELEASE>:NDEBUG>")

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /fp:fast /MP /EHa /Ot /W4 /Gy)
endif()

if(PLATFORM STREQUAL "WINDOWS")
    target_link_options(${PROJECT_NAME} PRIVATE /subsystem:windows /ENTRY:mainCRTStartup)
endif()

include(FetchContent)

message(STATUS "Fetching Venus....")

FetchContent_Declare(
    Venus
    GIT_REPOSITORY https://github.com/razerx100/Venus.git
    GIT_TAG master
)

FetchContent_MakeAvailable(Venus)

message(STATUS "Fetched Venus")

message(STATUS "Fetching Pluto....")

FetchContent_Declare(
    Pluto
    GIT_REPOSITORY https://github.com/razerx100/Pluto.git
    GIT_TAG master
)

FetchContent_MakeAvailable(Pluto)

message(STATUS "Fetched Pluto")

message(STATUS "Fetching GaiaX from the branch ${BRANCH}....")

FetchContent_Declare(
    GaiaX
    GIT_REPOSITORY https://github.com/razerx100/GaiaX.git
    GIT_TAG ${BRANCH}
)

FetchContent_MakeAvailable(GaiaX)

message(STATUS "Fetched GaiaX")

message(STATUS "Fetching Terra from the branch ${BRANCH}....")

FetchContent_Declare(
    Terra
    GIT_REPOSITORY https://github.com/razerx100/Terra.git
    GIT_TAG ${BRANCH}
)

FetchContent_MakeAvailable(Terra)

message(STATUS "Fetched Terra")

message(STATUS "Fetching Luna....")

FetchContent_Declare(
    Luna
    GIT_REPOSITORY https://github.com/razerx100/Luna.git
    GIT_TAG master
)

FetchContent_MakeAvailable(Luna)

message(STATUS "Fetched Luna")

message(STATUS "Fetching Europa from the branch ${BRANCH}....")

FetchContent_Declare(
    Europa
    GIT_REPOSITORY https://github.com/razerx100/Europa.git
    GIT_TAG ${BRANCH}
)

FetchContent_MakeAvailable(Europa)

message(STATUS "Fetched Europa")

target_link_libraries(${PROJECT_NAME} PUBLIC
    Venus Pluto GaiaX Terra Luna
)

add_dependencies(${PROJECT_NAME} GLSL HLSL)

set(ICON_PATH /resources/icon/Sol.ico)

file(
    GENERATE
    OUTPUT
     "buildIcon.cmake"
    CONTENT
     [[
       if(NOT EXISTS ${ICON_PATH})
           file(WRITE ${ICON_PATH})
       endif()
     ]]
)

add_custom_target(
    Icon
    ALL
    DEPENDS
     ${CMAKE_BINARY_DIR}/${ICON_PATH}
)

add_custom_command(
    TARGET
     Icon
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND}
     -DICON_PATH=${CMAKE_BINARY_DIR}/${ICON_PATH} -P buildIcon.cmake
)

add_custom_command(
    TARGET
     Icon
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
     -E copy ${CMAKE_BINARY_DIR}/${ICON_PATH} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/${ICON_PATH}
)

add_dependencies(${PROJECT_NAME} Icon)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
