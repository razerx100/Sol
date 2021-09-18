cmake_minimum_required(VERSION 3.11)

project(Sol
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")

if(IMGUIFLAG STREQUAL "ENABLE")
    file(GLOB_RECURSE SRC src/*.cpp includes/*.hpp ImGui/*.cpp ImGui/*.h)
else()
    file(GLOB_RECURSE SRC src/*.cpp includes/*.hpp)
endif()

if(MSVC)
    add_executable(${PROJECT_NAME} WIN32
        ${SRC}
        icon/win32ExecutableIcon.rc
    )
else()
    add_executable(${PROJECT_NAME}
        ${SRC}
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE includes/ includes/Graphics/ includes/Exceptions/ includes/Input/ external/)

target_compile_definitions(${PROJECT_NAME} PRIVATE "$<$<CONFIG:DEBUG>:_DEBUG>" "$<$<CONFIG:RELEASE>:NDEBUG>")

if(IMGUIFLAG STREQUAL "ENABLE")
    target_include_directories(${PROJECT_NAME} PRIVATE ImGui/includes/)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _IMGUI)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/libs/*.lib
)

if(MSVC)
    string(REPLACE "/W3 " "/W4 " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    string(REPLACE "/W3 " "/W4 " CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
    string(REPLACE "/W3 " "/W4 " CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
endif()

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /fp:fast /MP /EHa /Ot)
    target_link_options(${PROJECT_NAME} PRIVATE /subsystem:windows /ENTRY:mainCRTStartup)
    target_link_libraries(${PROJECT_NAME} PRIVATE
	d3d12.lib dxgi.lib dxguid.lib uuid.lib d3dcompiler.lib
    )
endif()

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
