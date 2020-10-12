cmake_minimum_required(VERSION 3.0)

project(tpOpenGL)

add_executable(${PROJECT_NAME} main.cpp)

find_package(glm REQUIRED)
if(GLM_FOUND)
	include_directories("${GLM_INCLUDE_DIR}")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DL_LIBS})

add_subdirectory(glad)
target_link_libraries(${PROJECT_NAME} PRIVATE glad)

find_package(glfw3 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

