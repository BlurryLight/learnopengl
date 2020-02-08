# Project definition
cmake_minimum_required(VERSION 3.1)
project(BRDF)

# Source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SOURCES "${SRC_DIR}/main.cpp" )
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package( assimp REQUIRED)

find_package(glsupport REQUIRED PATHS ${SRC_DIR}/../glsupport/)
find_package(glm REQUIRED)

IF(NOT GLSUPPORT_FOUND)
    message(${SRC_DIR})
    message(FATAL_ERROR "GLSUPPORT NOT FOUND")
ENDIF()
include_directories(${GLSUPPORT_INCLUDES})
# copy shaders

# it will copy when cmake runs
# my desire is to copy when building
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/brdf.vert
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/brdf.frag
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


# Executable definition and properties
add_executable(${PROJECT_NAME} ${SOURCES})
# target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")
target_link_libraries(${PROJECT_NAME} PRIVATE
  ${OPENGL_LIBRARIES}
  ${GLFW_LIBRARIES}
  ${CMAKE_DL_LIBS}
  ${GLSUPPORT_LIBRARIES}
  ${GLAD_LIBRARIES}
  ${GLM_LIBRARIES}
  dl
  ${ASSIMP_LIBRARIES}
  )
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
