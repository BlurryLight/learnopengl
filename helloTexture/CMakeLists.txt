# Project definition
cmake_minimum_required(VERSION 3.1)
project(InitGL)

# Source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SOURCES "${SRC_DIR}/main.cpp" "shader.h" "shader.cpp" "stb_image.h")
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

# copy shaders

# it will copy when cmake runs
# my desire is to copy when building
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/tri.vert
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/simple.frag
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/container.jpg
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/awesomeface.png
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Cmake will rerun whenever files are modified
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tri.vert
#       ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
#configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/simple.frag
#      ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

add_subdirectory(glad/)
# Executable definition and properties
add_executable(${PROJECT_NAME} ${SOURCES})
# target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")
target_link_libraries(${PROJECT_NAME} PRIVATE glad
  ${OPENGL_LIBRARIES}
  ${GLFW_LIBRARIES}
  ${CMAKE_DL_LIBS}
  )
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
