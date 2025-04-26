cmake_minimum_required(VERSION 3.21)

file(GLOB_RECURSE SRC src/*.cpp includes/*.hpp exports/*.hpp)

add_library(SolLib STATIC ${SRC})

target_include_directories(SolLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes/ ${CMAKE_CURRENT_SOURCE_DIR}/exports/)

if(MSVC)
    target_compile_options(SolLib PRIVATE /fp:fast /MP /Ot /W4 /Gy /std:c++latest /Zc:__cplusplus)

    target_compile_definitions(SolLib PUBLIC SOL_WIN32)
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

message(STATUS "Fetching Pluto from the branch ${BRANCH}....")

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

message(STATUS "Fetching Luna from the branch ${BRANCH}....")

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

message(STATUS "Fetching Callisto....")

FetchContent_Declare(
    Callisto
    GIT_REPOSITORY https://github.com/razerx100/Callisto.git
    GIT_TAG master
)

FetchContent_MakeAvailable(Callisto)

message(STATUS "Fetching DirectXMath....")

FetchContent_Declare(
    DirectXMath
    GIT_REPOSITORY https://github.com/razerx100/DirectXMath.git
    GIT_TAG master
)

FetchContent_MakeAvailable(DirectXMath)

message(STATUS "Fetched DirectXMath")

message(STATUS "Fetching STB....")

FetchContent_Declare(
    STB
    GIT_REPOSITORY https://github.com/razerx100/STB.git
    GIT_TAG master
)

FetchContent_MakeAvailable(STB)

message(STATUS "Fetched STB")

message(STATUS "Fetching TinyImporters....")

FetchContent_Declare(
    TinyImporters
    GIT_REPOSITORY https://github.com/razerx100/TinyImporters.git
    GIT_TAG master
)

FetchContent_MakeAvailable(TinyImporters)

message(STATUS "Fetched TinyImporters")

set(BUILD_SHARED_LIBS OFF)
set(ASSIMP_BUILD_ZLIB ON)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL_PDB OFF)

message(STATUS "Fetching Assimp....")

FetchContent_Declare(
    Assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG 53b5dba4df171373fddb5ce7f00db6399c0aeeaa
)

FetchContent_MakeAvailable(Assimp)

unset(BUILD_SHARED_LIBS)
unset(ASSIMP_BUILD_ZLIB)
unset(ASSIMP_BUILD_TESTS)
unset(ASSIMP_INSTALL_PDB)

message(STATUS "Fetched Assimp")

target_link_libraries(SolLib PUBLIC
    razer::venus razer::pluto razer::gaiaX razer::terra razer::luna razer::callisto razer::DxMath razer::stb razer::TinyImporter assimp
)

add_dependencies(SolLib GLSL HLSL)
