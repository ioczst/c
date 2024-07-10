##
# CMake minimum required version for the project.
##
cmake_minimum_required(VERSION 3.11)

project(player-sdl C)


find_package(SDL2 CONFIG REQUIRED)
find_package(FFMPEG REQUIRED)

set(CMAKE_C_STANDARD 99)


FILE(GLOB sources src/*.c)

add_executable(${CMAKE_PROJECT_NAME}  ${sources})

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${FFMPEG_INCLUDE_DIRS})


target_link_libraries(${CMAKE_PROJECT_NAME}
    PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    ${FFMPEG_LIBRARIES}
)
