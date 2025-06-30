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
#add wxWidgets
FetchContent_MakeAvailable(threepp)
#add ifcplusplus 
FetchContent_Declare(
        ifcplusplus
        GIT_REPOSITORY https://github.com/LongDepTrai98/ifcplusplus.git
)
FetchContent_MakeAvailable(ifcplusplus)
#add submodule 
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/lib)
#add include dirs ifcpp
set(IFCPP_INCLUDE_DIRS
    ${IFCPP_SOURCE_DIR}/IfcPlusPlus/src
    ${IFCPP_SOURCE_DIR}/IfcPlusPlus/src/ifcpp/IFC4X3/include
    ${IFCPP_SOURCE_DIR}/IfcPlusPlus/src/external
    ${IFCPP_SOURCE_DIR}/IfcPlusPlus/src/external/glm
    ${IFCPP_SOURCE_DIR}/IfcPlusPlus/src/external/Carve/include
    ${IFCPP_SOURCE_DIR}/IfcPlusPlus/src/external/Carve/src
    ${IFCPP_SOURCE_DIR}/IfcPlusPlus/src/external/Carve/src/include
    ${IFCPP_SOURCE_DIR}/IfcPlusPlus/src/external/Carve/src/common
    ${IFCPP_SOURCE_DIR}/IfcPlusPlus/src/external/Carve/build/src
)
#add include dirs web-ifc 
set(WEB_IFC_INCLUDE_DIRS 
    ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/tinynurbs-src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/fastfloat-src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/cdt-src/CDT/include
    ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/earcut-src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/spdlog-src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/stduuid-src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/tinycpptest-src/Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/lib/test
)
#project
set(PROJECT_NAME Dragon)
set(EDITOR_NAME_PROJECT App)
#set path 
set(WX_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/wxWidgets-3.3.0/include)
set(WX_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/wxWidgets-3.3.0/lib/vc_x64_lib/)
set(IFCPLUSPLUS_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/ifcplusplus-build/IfcPlusPlus)
set(WEB_IFC_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/3rdparty/lib)
project(${PROJECT_NAME})
#find_package(wxWidgets CONFIG REQUIRED)
find_package(glew CONFIG REQUIRED)
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
file(GLOB_RECURSE APP_CORE_SRC "src/core/*.cpp" "src/core/*.hpp")
source_group("core" FILES ${APP_CORE_SRC})
#renderer 
file(GLOB_RECURSE APP_RENDERER_SRC "src/renderer/*.cpp" "src/renderer/*.hpp")
source_group("renderer" FILES ${APP_RENDERER_SRC})
#controller
file(GLOB_RECURSE APP_CONTROLLER_SRC "src/controller/*.cpp" "src/controller/*.hpp")
source_group("controller" FILES ${APP_CONTROLLER_SRC})
#view port 
file(GLOB_RECURSE APP_VIEWPORT_SRC "src/view/*.cpp" "src/view/*.hpp")
source_group("view" FILES ${APP_VIEWPORT_SRC})
#geometry 
file(GLOB_RECURSE LOADER_SRC "src/loader/*.cpp" "src/loader/*.hpp")
source_group("loader" FILES ${LOADER_SRC})
 
add_executable(${EDITOR_NAME_PROJECT} 
	${CMAKE_CURRENT_SOURCE_DIR}/src/app.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/wxInclude.hpp
	${UI_SRC}
    ${UI_COMMANDS_SRC}
	${APP_CONFIG_SRC}
	${APP_CORE_SRC}
	${APP_RENDERER_SRC}
	${APP_CONTROLLER_SRC}
    ${APP_VIEWPORT_SRC}
    ${LOADER_SRC}
)

 #add include 
 target_include_directories(${EDITOR_NAME_PROJECT} 
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
	PRIVATE ${WX_INCLUDE_PATH}
	PRIVATE ${WX_INCLUDE_PATH}/msvc
    PRIVATE ${IFCPP_INCLUDE_DIRS}
    PRIVATE ${WEB_IFC_INCLUDE_DIRS}
 )
 #link lib 
target_link_libraries(${EDITOR_NAME_PROJECT}  
PRIVATE GLEW::glew
PRIVATE threepp::threepp
$<$<CONFIG:Debug>:IfcPlusPlusd.lib;/Debug/web-ifc-library.lib>
$<$<CONFIG:Release>:IfcPlusPlus.lib;/Release/web-ifc-library.lib>)
#directories lib
target_link_directories(${EDITOR_NAME_PROJECT}
PRIVATE ${WX_LIB_PATH}
PRIVATE ${IFCPLUSPLUS_LIB_PATH}
PRIVATE ${WEB_IFC_LIB_PATH})

target_compile_definitions(${EDITOR_NAME_PROJECT}
PRIVATE _UNICODE UNICODE _CRT_SECURE_NO_WARNINGS IFCQUERY_STATIC_LIB)