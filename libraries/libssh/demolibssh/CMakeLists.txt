cmake_minimum_required(VERSION 3.10)

project(demolibssh)

find_package(libssh CONFIG REQUIRED)
file (GLOB SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/*.c)


add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ssh)