cmake_minimum_required(VERSION 3.21)

file(GLOB_RECURSE SRC src/*.cc)

add_executable(SolTest
    ${SRC}
)

target_include_directories(SolTest PRIVATE ${SOL_PRIVATE_INCLUDES})

unset(SOL_PRIVATE_INCLUDES)

if(MSVC)
    target_compile_options(SolTest PRIVATE /fp:fast /MP /Ot /W4 /Gy)
endif()

include(FetchContent)

set(GOOGLE_TEST_COMMIT_ID 12a5852e451baabc79c63a86c634912c563d57bc CACHE STRING "Supply the latest commit ID from the GitHub repository.")

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/${GOOGLE_TEST_COMMIT_ID}.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

FetchContent_MakeAvailable(googletest)

target_link_libraries(SolTest PRIVATE
    GTest::gtest_main SolApp
    razer::venus Pluto razer::gaiaX razer::terra razer::luna razer::DxMath razer::stb
)

include(GoogleTest)

gtest_discover_tests(SolTest)
