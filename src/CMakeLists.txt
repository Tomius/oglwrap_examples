cmake_minimum_required(VERSION 2.8)

if (MSVC)
    link_libraries(opengl32)
else()
    link_libraries(GL)
endif()
link_libraries(glfw)
link_libraries(glad)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DUSE_DEBUG_CONTEXT -g")

if (BUILD_SHARED_LIBS)
    link_libraries("${MATH_LIBRARY}")
endif()

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set (LODEPNG_SOURCE "../deps/lodepng/lodepng.cpp")

file(GLOB EXAMPLE_01_SOURCE "cpp/01_square.cpp" "cpp/oglwrap_example.cpp")
set (EXAMPLE_01_BINARY_NAME "01_square")

file(GLOB EXAMPLE_02_SOURCE "cpp/02_textured_square.cpp" "cpp/oglwrap_example.cpp" ${LODEPNG_SOURCE})
set (EXAMPLE_02_BINARY_NAME "02_textured_square")

file(GLOB EXAMPLE_03_SOURCE "cpp/03_cube.cpp" "cpp/oglwrap_example.cpp")
set (EXAMPLE_03_BINARY_NAME "03_cube")

file(GLOB EXAMPLE_04_SOURCE "cpp/04_cylinder.cpp" "cpp/oglwrap_example.cpp")
set (EXAMPLE_04_BINARY_NAME "04_cylinder")

if (CMAKE_BUILD_TYPE MATCHES "RELEASE")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOGLWRAP_DEBUG=0")
endif()

add_executable(${EXAMPLE_01_BINARY_NAME} WIN32 ${EXAMPLE_01_SOURCE} ${ICON})
add_executable(${EXAMPLE_02_BINARY_NAME} WIN32 ${EXAMPLE_02_SOURCE} ${ICON})
add_executable(${EXAMPLE_03_BINARY_NAME} WIN32 ${EXAMPLE_03_SOURCE} ${ICON})
add_executable(${EXAMPLE_04_BINARY_NAME} WIN32 ${EXAMPLE_04_SOURCE} ${ICON})

set(WINDOWS_BINARIES ${EXAMPLE_01_BINARY_NAME} ${EXAMPLE_02_BINARY_NAME} ${EXAMPLE_03_BINARY_NAME} ${EXAMPLE_04_BINARY_NAME})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

