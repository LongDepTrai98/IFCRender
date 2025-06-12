cmake_minimum_required(VERSION 3.15)
include(FetchContent)
set(CMAKE_TOOLCHAIN_FILE "./vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
project(workflow LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Build options
option(BUILD_TESTS "Build test targets" ON)
option(USE_STATIC_LIBS "Use static libraries" OFF)
#add threepp 
set(THREEPP_BUILD_TESTS OFF)
set(THREEPP_BUILD_EXAMPLES OFF)
set(THREEPP_WITH_GLFW OFF) # Disable default GLFW backend as we use WxWidgets
FetchContent_Declare(
        threepp
        GIT_REPOSITORY https://github.com/markaren/threepp.git
)
FetchContent_MakeAvailable(threepp)
#project
set(PROJECT_NAME Dragon)
set(EDITOR_NAME_PROJECT App)
#set path 
set(WX_INCLUDE_PATH "D:\\GITHUB\\thirdparty\\wxWidgets-3.3.0\\include")
set(WX_LIB_PATH "D:\\GITHUB\\thirdparty\\wxWidgets-3.3.0\\lib\\vc_x64_lib")
project(${PROJECT_NAME})
#find_package(wxWidgets CONFIG REQUIRED)
find_package(glew CONFIG REQUIRED)
#add file 
#ui
file(GLOB_RECURSE UI_SRC "src/ui/*.cpp" "src/ui/*.hpp")
source_group("ui" FILES ${UI_SRC})
#config
file(GLOB_RECURSE APP_CONFIG_SRC "src/config/*.cpp" "src/config/*.hpp")
source_group("config" FILES ${APP_CONFIG_SRC})
#context 
file(GLOB_RECURSE APP_CONTEXT_SRC "src/core/context/*.cpp" "src/core/context/*.hpp")
source_group("core/context" FILES ${APP_CONTEXT_SRC})
add_executable(${EDITOR_NAME_PROJECT} 
	${CMAKE_CURRENT_SOURCE_DIR}/src/app.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/wxInclude.hpp
	${UI_SRC}
	${APP_CONFIG_SRC}
	${APP_CONTEXT_SRC})
 #add include 
 target_include_directories(${EDITOR_NAME_PROJECT} 
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
	PRIVATE ${WX_INCLUDE_PATH}
	PRIVATE ${WX_INCLUDE_PATH}/msvc
 )
target_link_libraries(${EDITOR_NAME_PROJECT}  
PRIVATE GLEW::glew
PRIVATE threepp::threepp)
target_compile_definitions(${EDITOR_NAME_PROJECT}
PRIVATE _UNICODE)