cmake_minimum_required(VERSION 3.21)

file(GLOB_RECURSE SRC src/*.cpp includes/*.hpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

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

target_include_directories(SolApp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/includes/)

if(MSVC)
    target_compile_options(SolApp PRIVATE /fp:fast /MP /Ot /W4 /Gy /std:c++latest /Zc:__cplusplus)

    target_link_options(SolApp PRIVATE /subsystem:windows /ENTRY:mainCRTStartup)
endif()

target_link_libraries(SolApp PRIVATE SolLib)

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

    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT SolApp)
endif()
