cmake_minimum_required(VERSION 3.6)
project(gl_test)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenGL REQUIRED)

if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)")
endif ()
if (CMAKE_SOURCE_DIR MATCHES " ")
    message(WARNING "Your Source Directory contains spaces.")
endif ()
if (CMAKE_BINARY_DIR MATCHES " ")
    message(WARNING "Your Build Directory contains spaces.")
endif ()

add_subdirectory(external)

if (INCLUDE_DISTRIB)
    add_subdirectory(distrib)
endif (INCLUDE_DISTRIB)

set(WIN32_EXECUTABLE true)

include_directories(
        external/glfw-3.1.2/include/
        external/glm-0.9.7.1/
        external/glew-1.13.0/include/
        .
)

set(
        ALL_LIBS
        ${OPENGL_LIBRARY}
        glfw
        GLEW_1130
)

add_definitions(
        -DTW_STATIC
        -DTW_NO_LIB_PRAGMA
        -DTW_NO_DIRECT3D
        -DGLEW_STATIC
        -D_CRT_SECURE_NO_WARNINGS
)

set(SOURCE_FILES main.cpp mesh.cpp mesh.hpp texture.cpp texture.h texture.hpp types.h window.cpp window.hpp)
add_executable(gl_test WIN32 ${SOURCE_FILES})

target_link_libraries(gl_test ${ALL_LIBS})