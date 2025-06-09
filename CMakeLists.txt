cmake_minimum_required(VERSION 3.15)
set(CMAKE_TOOLCHAIN_FILE "./vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
project(workflow LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Build options
option(BUILD_TESTS "Build test targets" ON)
option(USE_STATIC_LIBS "Use static libraries" OFF)
#project
set(PROJECT_NAME viewer)
set(EDITOR_NAME_PROJECT editor)
project(${PROJECT_NAME})
find_package(wxWidgets CONFIG REQUIRED)
find_package(glew CONFIG REQUIRED)
#add file 
file(GLOB_RECURSE UI_SRC "src/ui/*.cpp" "src/ui/*.hpp")
source_group("ui" FILES ${UI_SRC})
file(GLOB_RECURSE APP_CONFIG_SRC "src/config/*.cpp" "src/config/*.hpp")
source_group("config" FILES ${APP_CONFIG_SRC})
add_executable(${EDITOR_NAME_PROJECT} 
main.cpp
 ${UI_SRC}
 ${APP_CONFIG_SRC})
 #add include 
 target_include_directories(${EDITOR_NAME_PROJECT} 
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
 )
target_link_libraries(${EDITOR_NAME_PROJECT} PRIVATE wx::core wx::base wx::wxribbon wx::wxaui wx::wxgl 
PRIVATE GLEW::glew)
