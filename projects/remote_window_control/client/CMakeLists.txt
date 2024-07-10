cmake_minimum_required(VERSION 3.10)

project(client)

find_package(fmt CONFIG REQUIRED)
find_package(OpenCV REQUIRED)

set(IMGUI_PATH libs/imgui)
file(GLOB IMGUI_SOURCE_FILES ${IMGUI_PATH}/imgui*.cpp)
file(GLOB MAIN_SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${MAIN_SOURCE_FILES} ${IMGUI_SOURCE_FILES} ${IMGUI_PATH}/backends/imgui_impl_glfw.cpp ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp )

target_link_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/libs/glfw/lib-vc2010-64)

target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include ${IMGUI_PATH} ${IMGUI_PATH}/backends libs/glfw/include)
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBRARIES} glfw3 opengl32 fmt::fmt)


