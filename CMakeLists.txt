cmake_minimum_required(VERSION 3.15)
include(FetchContent)
set(CMAKE_TOOLCHAIN_FILE "./vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
cmake_policy(SET CMP0142 NEW)
#build type 
 set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
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
#add wxWidgets
#add submodule 
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/lib)
#add include dirs web-ifc 
set(WEB_IFC_INCLUDE_DIRS 
    ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/tinynurbs-src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/fastfloat-src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/cdt-src/CDT/include
    ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/earcut-src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/spdlog-src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/stduuid-src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/glm-src/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/glm-src
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/lib/test
)
#project
set(PROJECT_NAME Dragon)
set(EDITOR_NAME_PROJECT App)
#set path 
set(WX_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/wxWidgets-3.3.0/include)
set(WX_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/wxWidgets-3.3.0/lib/vc_x64_lib/)
set(WEB_IFC_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/3rdparty/lib)
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
set(DEST_ASSETS_DIR "$<TARGET_FILE_DIR:${EDITOR_NAME_PROJECT}>/assets")
set(DEBUG_ASSETS_DIR "${CMAKE_BINARY_DIR}/assets")
project(${PROJECT_NAME})
#find_package(wxWidgets CONFIG REQUIRED)
find_package(glew CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
#add file 
#ui
file(GLOB_RECURSE UI_SRC "src/ui/*.cpp" "src/ui/*.hpp")
source_group("ui" FILES ${UI_SRC})
#app commands 
file(GLOB_RECURSE UI_COMMANDS_SRC "src/commands/*.cpp" "src/commands/*.hpp")
source_group("commands" FILES ${UI_COMMANDS_SRC})
#config
file(GLOB_RECURSE APP_CONFIG_SRC "src/config/*.cpp" "src/config/*.hpp")
source_group("config" FILES ${APP_CONFIG_SRC})
#core 
file(GLOB APP_CORE_SRC "src/core/*.cpp" "src/core/*.hpp")
source_group("core" FILES ${APP_CORE_SRC})
#convert 
file(GLOB APP_CONVERT_CORE_SRC "src/core/convert/*.cpp" "src/core/convert/*.hpp")
source_group("core//convert" FILES ${APP_CONVERT_CORE_SRC})
#io
file(GLOB_RECURSE APP_IO_CORE_SRC "src/core/io/*.cpp" "src/core/io/*.hpp")
source_group("core//io" FILES ${APP_IO_CORE_SRC})
#node
file(GLOB APP_NODE_CORE_SRC "src/core/node/*.cpp" "src/core/node/*.hpp")
source_group("core//node" FILES ${APP_NODE_CORE_SRC})
#props
file(GLOB APP_PROPS_CORE_SRC "src/core/props/*.cpp" "src/core/props/*.hpp")
source_group("core//props" FILES ${APP_PROPS_CORE_SRC})
#utils
file(GLOB APP_UTILS_CORE_SRC "src/core/utils/*.cpp" "src/core/utils/*.hpp")
source_group("core//utils" FILES ${APP_UTILS_CORE_SRC})
#renderer 
file(GLOB_RECURSE APP_RENDERER_SRC "src/renderer/*.cpp" "src/renderer/*.hpp")
source_group("renderer" FILES ${APP_RENDERER_SRC})
#view port 
file(GLOB_RECURSE APP_VIEWPORT_SRC "src/view/*.cpp" "src/view/*.hpp")
source_group("view" FILES ${APP_VIEWPORT_SRC})
#geometry 
file(GLOB_RECURSE RAYCAST_SRC "src/raycast/*.cpp" "src/raycast/*.hpp")
source_group("raycast" FILES ${RAYCAST_SRC})
#tsl
file(GLOB_RECURSE TSL_SRC "src/tsl/*.h")
source_group("tsl" FILES ${TSL_SRC})
#input 
file(GLOB_RECURSE INPUT_SRC "src/input/*.hpp")
source_group("input" FILES ${INPUT_SRC})
#threadpool 
 file(GLOB_RECURSE THREADPOOL_SRC "src/threadpool/*.hpp")
source_group("threadpool" FILES ${THREADPOOL_SRC})


add_executable(${EDITOR_NAME_PROJECT} 
	${CMAKE_CURRENT_SOURCE_DIR}/src/app.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/wxInclude.hpp
	${UI_SRC}
    ${UI_COMMANDS_SRC}
	${APP_CONFIG_SRC}
	${APP_CORE_SRC}
    ${APP_CONVERT_CORE_SRC}
    ${APP_IO_CORE_SRC}
    ${APP_NODE_CORE_SRC}
    ${APP_PROPS_CORE_SRC}
    ${APP_UTILS_CORE_SRC}
	${APP_RENDERER_SRC}
    ${APP_VIEWPORT_SRC}
    ${RAYCAST_SRC}
    ${TSL_SRC}
    ${INPUT_SRC}
    ${THREADPOOL_SRC}
)

 #add include 
 target_include_directories(${EDITOR_NAME_PROJECT} 
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
	PRIVATE ${WX_INCLUDE_PATH}
	PRIVATE ${WX_INCLUDE_PATH}/msvc
    PRIVATE ${WEB_IFC_INCLUDE_DIRS}
 )
 #link lib 
target_link_libraries(${EDITOR_NAME_PROJECT}  
PRIVATE GLEW::glew
PRIVATE threepp::threepp
PRIVATE nlohmann_json
$<$<CONFIG:Debug>:/Debug/web-ifc-library.lib;wxpngd.lib;wxwebpd.lib;wxjpegd.lib>
$<$<CONFIG:Release>:/Release/web-ifc-library.lib;wxpng.lib;wxwebp.lib;wxjpeg.lib>)
#directories lib
target_link_directories(${EDITOR_NAME_PROJECT}
PRIVATE ${WX_LIB_PATH}
PRIVATE ${WEB_IFC_LIB_PATH})

target_compile_definitions(${EDITOR_NAME_PROJECT}
PRIVATE _UNICODE UNICODE _CRT_SECURE_NO_WARNINGS NANORT_ENABLE_PARALLEL_BUILD FMT_UNICODE=0  NANORT_USE_CPP11_FEATURE=1)

# Custom command to copy assets
add_custom_command(
    TARGET ${EDITOR_NAME_PROJECT} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ASSETS_DIR}"
            "${DEST_ASSETS_DIR}"
    COMMENT "Copying assets folder to output directory..."
)

add_custom_command(
    TARGET ${EDITOR_NAME_PROJECT} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ASSETS_DIR}"
            "${DEBUG_ASSETS_DIR}"
    COMMENT "Copying assets folder to output directory..."
)
