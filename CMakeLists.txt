cmake_minimum_required(VERSION 3.4)
project(FactorioViewer)

find_package(Lua REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O0")

include_directories(include)
include_directories(/usr/include/lua)
include_directories(libs/tclap-1.2.1/include)

set(SOURCE_FILES src/main.cpp src/ASCIITree.cpp include/ASCIITree.h include/util.hpp src/DotFormatter.cpp include/DotFormatter.h include/Settings.h src/Settings.cpp)
add_executable(FactorioViewer ${SOURCE_FILES})

IF (LUA_FOUND)
    include_directories(${LUA_INCLUDE_DIR})
    target_link_libraries(FactorioViewer ${LUA_LIBRARIES})
endif()