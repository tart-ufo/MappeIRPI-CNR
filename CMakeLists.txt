cmake_minimum_required(VERSION 3.12)
project(MappeIrpi)

SET(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Ofast ${CMAKE_CXX_FLAGS} -fopenmp -lstdc++fs ")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg -g")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
set(SOURCE_FILES main.cpp)
add_executable(MappeIrpi ${SOURCE_FILES})

FIND_PACKAGE(GDAL COMPONENTS REQUIRED)

target_link_libraries(MappeIrpi gdal stdc++fs)