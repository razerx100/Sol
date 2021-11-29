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

target_include_directories(${PROJECT_NAME} PRIVATE includes/ exports/ templates/ includes/Impl/)

target_compile_definitions(${PROJECT_NAME} PRIVATE "$<$<CONFIG:DEBUG>:_DEBUG>" "$<$<CONFIG:RELEASE>:NDEBUG>")

if(IMGUIFLAG STREQUAL "ENABLE")
    target_include_directories(${PROJECT_NAME} PRIVATE ImGui/includes/)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _IMGUI)
endif()

if(MSVC)
    string(REPLACE "/W3 " "/W4 " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    string(REPLACE "/W3 " "/W4 " CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
    string(REPLACE "/W3 " "/W4 " CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
endif()

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /fp:fast /MP /EHa /Ot)
endif()

if(PLATFORM STREQUAL "WINDOWS")
    target_link_options(${PROJECT_NAME} PRIVATE /subsystem:windows /ENTRY:mainCRTStartup)
endif()

include(FetchContent)

FetchContent_Declare(
    ${INPUT_MANAGER}
    GIT_REPOSITORY https://github.com/razerx100/Pluto.git
    GIT_TAG master
)

FetchContent_MakeAvailable(${INPUT_MANAGER})

if(GRAPHICS_ENGINE STREQUAL "GaiaX")
    FetchContent_Declare(
        ${GRAPHICS_ENGINE}
        GIT_REPOSITORY https://github.com/razerx100/GaiaX.git
        GIT_TAG master
    )

    FetchContent_MakeAvailable(${GRAPHICS_ENGINE})
elseif(GRAPHICS_ENGINE STREQUAL "Terra")
    FetchContent_Declare(
        ${GRAPHICS_ENGINE}
        GIT_REPOSITORY https://github.com/razerx100/Terra.git
        GIT_TAG master
    )

    FetchContent_MakeAvailable(${GRAPHICS_ENGINE})
endif()

if(PLATFORM STREQUAL "WINDOWS")
    FetchContent_Declare(
        ${RENDER_WINDOW}
        GIT_REPOSITORY https://github.com/razerx100/Luna.git
        GIT_TAG master
    )

    FetchContent_MakeAvailable(${RENDER_WINDOW})
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC
    ${CMAKE_BINARY_DIR}/lib/$<$<CONFIG:DEBUG>:Debug>$<$<CONFIG:RELEASE>:Release>/InputManager.lib
    ${CMAKE_BINARY_DIR}/lib/$<$<CONFIG:DEBUG>:Debug>$<$<CONFIG:RELEASE>:Release>/GraphicsEngine.lib
    ${CMAKE_BINARY_DIR}/lib/$<$<CONFIG:DEBUG>:Debug>$<$<CONFIG:RELEASE>:Release>/RenderWindow.lib
)

add_dependencies(${PROJECT_NAME} ${INPUT_MANAGER} ${GRAPHICS_ENGINE} ${RENDER_WINDOW})

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
