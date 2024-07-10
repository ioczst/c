cmake_minimum_required(VERSION 3.10)

project(server)

find_package(fmt CONFIG REQUIRED)
find_package(OpenCV REQUIRED)

file (GLOB SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBRARIES} fmt::fmt)
