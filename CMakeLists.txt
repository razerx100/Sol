cmake_minimum_required(VERSION 3.21)

project(Sol
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
endif()

if(IMGUIFLAG STREQUAL "ENABLE")
    file(GLOB_RECURSE SRC src/*.cpp includes/*.hpp exports/*.hpp templates/*.hpp ImGui/*.cpp ImGui/*.h)
else()
    file(GLOB_RECURSE SRC src/*.cpp includes/*.hpp exports/*.hpp templates/*.hpp)
endif()

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

if(NOT CERES_ADDED)
    add_subdirectory(Ceres)
    set(CERES_ADDED "TRUE")
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC Ceres)

target_include_directories(${PROJECT_NAME} PRIVATE includes/ exports/ templates/ templates/modules/)

target_compile_definitions(${PROJECT_NAME} PRIVATE "$<$<CONFIG:DEBUG>:_DEBUG>" "$<$<CONFIG:RELEASE>:NDEBUG>")

if(IMGUIFLAG STREQUAL "ENABLE")
    target_include_directories(${PROJECT_NAME} PRIVATE ImGui/includes/)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _IMGUI)
endif()

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /fp:fast /MP /EHa /Ot /W4)
endif()

if(PLATFORM STREQUAL "WINDOWS")
    target_link_options(${PROJECT_NAME} PRIVATE /subsystem:windows /ENTRY:mainCRTStartup)
endif()

include(FetchContent)

FetchContent_Declare(
    Venus
    GIT_REPOSITORY https://github.com/razerx100/Venus.git
    GIT_TAG master
)

FetchContent_MakeAvailable(Venus)

FetchContent_Declare(
    Pluto
    GIT_REPOSITORY https://github.com/razerx100/Pluto.git
    GIT_TAG master
)

FetchContent_MakeAvailable(Pluto)

FetchContent_Declare(
    GaiaX
    GIT_REPOSITORY https://github.com/razerx100/GaiaX.git
    GIT_TAG ${BRANCH}
)

FetchContent_MakeAvailable(GaiaX)

FetchContent_Declare(
    Terra
    GIT_REPOSITORY https://github.com/razerx100/Terra.git
    GIT_TAG ${BRANCH}
)

FetchContent_MakeAvailable(Terra)

FetchContent_Declare(
    Luna
    GIT_REPOSITORY https://github.com/razerx100/Luna.git
    GIT_TAG master
)

FetchContent_MakeAvailable(Luna)

target_link_libraries(${PROJECT_NAME} PUBLIC
    Venus Pluto GaiaX Terra Luna
)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
