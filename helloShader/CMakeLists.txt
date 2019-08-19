# Project definition
cmake_minimum_required(VERSION 3.1)
project(InitGL)

# Source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SOURCES "${SRC_DIR}/main.cpp")
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

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
