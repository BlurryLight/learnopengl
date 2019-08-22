# Project definition
cmake_minimum_required(VERSION 3.1)
project(glsupport)

# Source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SOURCES  "glsupport.h" "glsupport.cpp")
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

add_subdirectory(glad/)
# add library
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE glad
  ${OPENGL_LIBRARIES}
  ${GLFW_LIBRARIES}
  ${CMAKE_DL_LIBS}
  )
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
