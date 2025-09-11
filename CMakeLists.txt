cmake_minimum_required(VERSION 3.15)
include(FetchContent)
include(ExternalProject)
set(CMAKE_TOOLCHAIN_FILE "./vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
cmake_policy(SET CMP0142 NEW)

#cesium 
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
set_target_properties(threepp PROPERTIES FOLDER "render")
#add cesium-native
set(DCESIUM_USE_EZVCPG OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cesium-native)
if(NOT CESIUM_DISABLE_CURL) 
    list(APPEND cesium_native_targets CesiumCurl)
endif()
set(cesium_native_targets
    Cesium3DTiles
    Cesium3DTilesContent
    Cesium3DTilesReader
    Cesium3DTilesSelection
    Cesium3DTilesWriter
    CesiumAsync
    CesiumGeometry
    CesiumGeospatial
    CesiumGltf
    CesiumGltfContent
    CesiumGltfReader
    CesiumGltfWriter
    CesiumIonClient
    CesiumITwinClient
    CesiumVectorData
    CesiumQuantizedMeshTerrain
    CesiumRasterOverlays
    CesiumUtility
)
set(cesium_include_directories "")
foreach(target ${cesium_native_targets})
    get_target_property(_public_private_include_directories ${target} INCLUDE_DIRECTORIES)
    get_target_property(_interface_include_directories ${target} INTERFACE_INCLUDE_DIRECTORIES)
    list(APPEND _private_include_directories ${_public_private_include_directories})
    list(REMOVE_ITEM _private_include_directories ${_interface_include_directories})
    list(APPEND cesium_include_directories ${_private_include_directories})
endforeach()
foreach(tgt IN LISTS cesium_native_targets)
    set_target_properties(${tgt} PROPERTIES FOLDER "cesium-native")
endforeach()
set_target_properties(CesiumClientCommon PROPERTIES FOLDER "cesium-native")
set_target_properties(CesiumCurl PROPERTIES FOLDER "cesium-native")
set_target_properties(CesiumJsonReader PROPERTIES FOLDER "cesium-native")
set_target_properties(CesiumJsonWriter PROPERTIES FOLDER "cesium-native")
message(${cesium_include_directories})

#add include dirs web-ifc 
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/lib)
set(WEB_IFC_INCLUDE_DIRS 
    ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/tinynurbs-src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/fastfloat-src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/cdt-src/CDT/include
    ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/earcut-src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/stduuid-src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/lib/test
)
set_target_properties(web-ifc PROPERTIES FOLDER "ifc")
set_target_properties(web-ifc-library PROPERTIES FOLDER "ifc")
set_target_properties(web-ifc-test PROPERTIES FOLDER "ifc")
#add maplibre 
set(MAPLIBRE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/maplibre)
if(NOT EXISTS ${MAPLIBRE_SOURCE_DIR}/CMakeLists.txt)
    execute_process(
        COMMAND git clone --config core.longpaths=true --depth 1 --recurse-submodules -j8
                https://github.com/LongDepTrai98/maplibre-native.git
                ${MAPLIBRE_SOURCE_DIR}
    )
endif()
set(MAPLIBRE_INCLUDE_SOURCE_DIRS
    ${MAPLIBRE_SOURCE_DIR}/src
    ${MAPLIBRE_SOURCE_DIR}/include
    ${MAPLIBRE_SOURCE_DIR}/platform/default/include
    ${MAPLIBRE_SOURCE_DIR}/vendor/maplibre-native-base/deps/geometry.hpp/include
    ${MAPLIBRE_SOURCE_DIR}/vendor/maplibre-native-base/deps/cheap-ruler-cpp/include
    ${MAPLIBRE_SOURCE_DIR}/vendor/maplibre-native-base/deps/variant/include
    ${MAPLIBRE_SOURCE_DIR}/vendor/maplibre-native-base/deps/supercluster.hpp/include
    ${MAPLIBRE_SOURCE_DIR}/vendor/maplibre-native-base/deps/shelf-pack-cpp/include
    ${MAPLIBRE_SOURCE_DIR}/vendor/maplibre-native-base/deps/pixelmatch-cpp/include
    ${MAPLIBRE_SOURCE_DIR}/vendor/maplibre-native-base/deps/jni.hpp/include
    ${MAPLIBRE_SOURCE_DIR}/vendor/maplibre-native-base/deps/geojson-vt-cpp/include
    ${MAPLIBRE_SOURCE_DIR}/vendor/maplibre-native-base/deps/geojson.hpp/include
    ${MAPLIBRE_SOURCE_DIR}/vendor/maplibre-native-base/include
    ${MAPLIBRE_SOURCE_DIR}/vendor/maplibre-native-base/extras/expected-lite/include
    ${MAPLIBRE_SOURCE_DIR}/vendor/maplibre-native-base/extras/rapidjson/include
    ${MAPLIBRE_SOURCE_DIR}/vendor/nontype_functional/include
    ${MAPLIBRE_SOURCE_DIR}/vendor/unique_resource)
#add_subdirectory(${MAPLIBRE_SOURCE_DIR})
#project
set(PROJECT_NAME Dragon)
set(EDITOR_NAME_PROJECT App)
#set path 
set(WX_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/wxWidgets-3.3.1/include)
set(WX_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/wxWidgets-3.3.1/lib/vc_x64_lib/)
set(WEB_IFC_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/3rdparty/lib)
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
set(SHADER_DIR "${CMAKE_SOURCE_DIR}/src/shaders")
set(DEST_SHADER_DIR "${CMAKE_BINARY_DIR}/shaders")
set(DEST_ASSETS_DIR "$<TARGET_FILE_DIR:${EDITOR_NAME_PROJECT}>/assets")
set(DEBUG_ASSETS_DIR "${CMAKE_BINARY_DIR}/assets")
project(${PROJECT_NAME})
#find_package(wxWidgets CONFIG REQUIRED)
#find_package(glew CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(s2 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
#find_package(assimp CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
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
#vector-tile 
file(GLOB APP_VECTOR_TILE_CORE_SRC "src/core/utils/vector-tile/*.cpp" "src/core/utils/vector-tile/*.hpp")
source_group("core//utils//vector-tile" FILES ${APP_VECTOR_TILE_CORE_SRC})
#draw pass 
file(GLOB APP_DRAWPASS_CORE_SRC "src/core/drawpass/*.cpp" "src/core/drawpass/*.hpp")
source_group("core//drawpass" FILES ${APP_DRAWPASS_CORE_SRC})
#lock 
file(GLOB APP_CONTEXT_LOCK "src/core/lock/*.cpp" "src/core/lock/*.hpp")
source_group("core//lock" FILES ${APP_CONTEXT_LOCK})
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
#shader 
file(GLOB_RECURSE SHADERS_SRC "src/shaders/*.vert" "src/shaders/*.frag")
source_group("shaders" FILES ${SHADERS_SRC})
#maplibre 
file(GLOB MAPLIBRE_SRC "src/map/*.hpp" "src/map/*.cpp")
source_group("map" FILES ${MAPLIBRE_SRC})
#cesium
file(GLOB MAPLIBRE_CESIUM_SRC "src/map/cesium/*.hpp" "src/map/cesium/*.cpp")
source_group("map//cesium" FILES ${MAPLIBRE_CESIUM_SRC})
#tool 
file(GLOB_RECURSE TOOL_SRC "src/tools/*.hpp" "src/tools/*.cpp")
source_group("tools" FILES ${TOOL_SRC})
add_executable(${EDITOR_NAME_PROJECT} 
	${CMAKE_CURRENT_SOURCE_DIR}/src/app.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/wxInclude.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/resource.hpp
	${UI_SRC}
    ${UI_COMMANDS_SRC}
	${APP_CONFIG_SRC}
	${APP_CORE_SRC}
    ${APP_CONVERT_CORE_SRC}
    ${APP_IO_CORE_SRC}
    ${APP_NODE_CORE_SRC}
    ${APP_PROPS_CORE_SRC}
    ${APP_UTILS_CORE_SRC}
    ${APP_DRAWPASS_CORE_SRC}
	${APP_RENDERER_SRC}
    ${APP_VIEWPORT_SRC}
    ${RAYCAST_SRC}
    ${TSL_SRC}
    ${INPUT_SRC}
    ${THREADPOOL_SRC}
    ${SHADERS_SRC}
    ${MAPLIBRE_SRC}
    ${APP_CONTEXT_LOCK}
    ${APP_VECTOR_TILE_CORE_SRC}
    ${TOOL_SRC}
    ${MAPLIBRE_CESIUM_SRC}
)

 #add include 
 target_include_directories(${EDITOR_NAME_PROJECT} 
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
	PRIVATE ${WX_INCLUDE_PATH}
	PRIVATE ${WX_INCLUDE_PATH}/msvc
    PRIVATE ${WEB_IFC_INCLUDE_DIRS}
    PRIVATE ${MAPLIBRE_INCLUDE_SOURCE_DIRS}
    PRIVATE ${cesium_include_directories}
 )
 #link lib 
target_link_libraries(${EDITOR_NAME_PROJECT}  
#PRIVATE GLEW::glew
PUBLIC ${cesium_native_targets}
PRIVATE threepp::threepp
PRIVATE nlohmann_json
PRIVATE s2::s2
PRIVATE fmt::fmt
PRIVATE cpr::cpr
$<$<CONFIG:Debug>:/Debug/web-ifc-library.lib>
$<$<CONFIG:Release>:/Release/web-ifc-library.lib>
$<$<CONFIG:Debug>:${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/debug/lib/libuv.lib
;${MAPLIBRE_SOURCE_DIR}/build/Debug/mbgl-core.lib;${MAPLIBRE_SOURCE_DIR}/build/Debug/mbgl-vendor-parsedate.lib
;${MAPLIBRE_SOURCE_DIR}/build/Debug/mbgl-vendor-csscolorparser.lib;${MAPLIBRE_SOURCE_DIR}/build/vendor/harfbuzz/Debug/harfbuzz.lib
;${MAPLIBRE_SOURCE_DIR}/build/vendor/freetype/Debug/freetyped.lib
;opengl32.lib;ws2_32.lib; bcrypt.lib;crypt32.lib;psapi.lib;user32.lib;iphlpapi.lib;userenv.lib;dbghelp.lib
;${MAPLIBRE_SOURCE_DIR}/build/Debug/mbgl-vendor-sqlite.lib;${MAPLIBRE_SOURCE_DIR}/build/Debug/mbgl-vendor-nunicode.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/debug/lib/libpng16d.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/debug/lib/libcurl-d.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/debug/lib/zlibd.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/debug/lib/dl.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/debug/lib/jpeg.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/debug/lib/libwebpdecoder.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/debug/lib/libwebp.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/debug/lib/libwebpdemux.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/debug/lib/libwebpmux.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/debug/lib/libsharpyuv.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/debug/lib/icuind.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/debug/lib/icuucd.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/debug/lib/icudtd.lib>
$<$<CONFIG:Release>:${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/lib/libuv.lib
;${MAPLIBRE_SOURCE_DIR}/build/Release/mbgl-core.lib
;${MAPLIBRE_SOURCE_DIR}/build/Release/mbgl-vendor-parsedate.lib;${MAPLIBRE_SOURCE_DIR}/build/Release/mbgl-vendor-csscolorparser.lib
;${MAPLIBRE_SOURCE_DIR}/build/Release/mbgl-vendor-nunicode.lib;${MAPLIBRE_SOURCE_DIR}/build/Release/mbgl-vendor-sqlite.lib
;${MAPLIBRE_SOURCE_DIR}/build/vendor/harfbuzz/Release/harfbuzz.lib;${MAPLIBRE_SOURCE_DIR}/build/vendor/freetype/Release/freetype.lib
;opengl32.lib;ws2_32.lib; bcrypt.lib;crypt32.lib;psapi.lib;user32.lib;iphlpapi.lib;userenv.lib;dbghelp.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/lib/libpng16.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/lib/libcurl.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/lib/zlib.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/lib/dl.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/lib/jpeg.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/lib/libwebpdecoder.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/lib/libwebp.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/lib/libwebpdemux.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/lib/libwebpmux.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/lib/libsharpyuv.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/lib/icuin.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/lib/icuuc.lib
;${MAPLIBRE_SOURCE_DIR}/platform/windows/vendor/vcpkg/installed/x64-windows/lib/icudt.lib
;>)
#directories lib
target_link_directories(${EDITOR_NAME_PROJECT}
PRIVATE ${WX_LIB_PATH}
PRIVATE ${WEB_IFC_LIB_PATH})

target_compile_definitions(${EDITOR_NAME_PROJECT}
PRIVATE _UNICODE UNICODE _USE_MATH_DEFINES _CRT_SECURE_NO_WARNINGS NANORT_ENABLE_PARALLEL_BUILD FMT_UNICODE=0  NANORT_USE_CPP11_FEATURE=1 SPDLOG_COMPILED_LIB SPDLOG_HEADER_ONLY _SILENCE_CXX20_OLD_SHARED_PTR_WARNING)

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
            "${SHADER_DIR}"
            "${DEST_SHADER_DIR}"
    COMMENT "Copying shader folder to output directory..."
)


add_custom_command(
    TARGET ${EDITOR_NAME_PROJECT} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
           "${ASSETS_DIR}"
           "${DEBUG_ASSETS_DIR}"
    COMMENT "Copying assets folder to debug directory..."
)


