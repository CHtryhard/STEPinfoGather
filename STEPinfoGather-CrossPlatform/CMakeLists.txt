cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_TOOLCHAIN_FILE "D:/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(VCPKG_TARGET_TRIPLET "x64-windows-static")
#try out cmake
# set(VCPKG_LIBRARY_LINKAGE static)

if(APPLE AND NOT CMAKE_OSX_DEPLOYMENT_TARGET)
    # If no deployment target has been set default to the minimum supported
    # OS version (this has to be set before the first project() call)
    if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
        set(CMAKE_OSX_DEPLOYMENT_TARGET 12.0 CACHE STRING "iOS Deployment Target")
    else()
        set(CMAKE_OSX_DEPLOYMENT_TARGET 10.10 CACHE STRING "macOS Deployment Target")
    endif()
endif()

project(step_infor_exporter CXX)

cmake_policy(SET CMP0091 NEW) 

set(BUILD_SHARED_LIBS OFF)
find_package(wxwidgets CONFIG REQUIRED)
find_package(OpenCASCADE CONFIG REQUIRED)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Set the MSVC runtime library to static
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_executable(example WIN32 MACOSX_BUNDLE src/App.cpp src/MainFrame.cpp)
target_include_directories(example PUBLIC src)
target_link_libraries(example PRIVATE  "TKBin"     "TKCAF"       "TKDEPLY"  "TKG2d"      "TKMesh"    "TKShHealing" "TKXCAF"
"TKBinL"    "TKCDF"       "TKDESTEP" "TKG3d"      "TKMeshVS"  "TKStd"       "TKXMesh"
"TKBinTObj" "TKDE"        "TKDESTL"  "TKGeomAlgo" "TKOffset"  "TKStdL"      "TKXml"
"TKBinXCAF" "TKDECascade" "TKDEVRML" "TKGeomBase" "TKOpenGl"  "TKTObj"      "TKXmlL"
"TKBO"      "TKDEGLTF"    "TKernel"  "TKHLR"      "TKPrim"    "TKTopAlgo"   "TKXmlTObj"
"TKBool"    "TKDEIGES"    "TKFeat"   "TKLCAF"     "TKRWMesh"  "TKV3d"       "TKXmlXCAF"
"TKBRep"    "TKDEOBJ"     "TKFillet" "TKMath"     "TKService" "TKVCAF"      "TKXSBase")
target_link_libraries(example PRIVATE wx::wxbase wx::wxcore)