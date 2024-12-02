cmake_minimum_required(VERSION 3.21)

file(GLOB_RECURSE SRC src/*.cpp includes/*.hpp exports/*.hpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

if(ADD_TEST_SOL)
    add_library(SolApp STATIC ${SRC})
else()
    if(MSVC)
        add_executable(SolApp WIN32
            ${SRC}
            # icon/win32ExecutableIcon.rc
        )
    else()
        add_executable(SolApp
            ${SRC}
        )
    endif()
endif()

set(SOL_PRIVATE_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/includes/ ${CMAKE_CURRENT_SOURCE_DIR}/exports/ ${CMAKE_CURRENT_SOURCE_DIR}/includes/Exceptions/)

set(SOL_PRIVATE_INCLUDES ${SOL_PRIVATE_INCLUDES} PARENT_SCOPE)

target_include_directories(SolApp PRIVATE ${SOL_PRIVATE_INCLUDES})

if(MSVC)
    target_compile_options(SolApp PRIVATE /fp:fast /MP /Ot /W4 /Gy)

   if(NOT ADD_TEST_SOL)
       target_link_options(SolApp PRIVATE /subsystem:windows /ENTRY:mainCRTStartup)
   endif()
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

set(BUILD_SHARED_LIBS OFF)
set(ASSIMP_BUILD_ZLIB ON)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL_PDB OFF)

message(STATUS "Fetching Assimp....")

FetchContent_Declare(
    Assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG master
)

FetchContent_MakeAvailable(Assimp)

unset(BUILD_SHARED_LIBS)
unset(ASSIMP_BUILD_ZLIB)
unset(ASSIMP_BUILD_TESTS)
unset(ASSIMP_INSTALL_PDB)

message(STATUS "Fetched Assimp")

target_link_libraries(SolApp PRIVATE
    razer::venus razer::pluto razer::gaiaX razer::terra razer::luna razer::DxMath razer::stb assimp
)

add_dependencies(SolApp GLSL HLSL)

if (MSVC)
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

    add_dependencies(SolApp Icon)

    if(NOT ADD_TEST_SOL)
        set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT SolApp)
    endif()
endif()
