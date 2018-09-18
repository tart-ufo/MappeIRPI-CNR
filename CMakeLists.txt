cmake_minimum_required(VERSION 3.12)
project(MappeIrpi)

SET(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SOURCE_FILES main.cpp)
add_executable(MappeIrpi ${SOURCE_FILES})

FIND_PACKAGE(GDAL COMPONENTS REQUIRED)
add_definitions(-DMAGICKCORE_QUANTUM_DEPTH=8)
add_definitions( -DMAGICKCORE_HDRI_ENABLE=0 )
find_package(ImageMagick COMPONENTS Magick++)
include_directories(${ImageMagick_INCLUDE_DIRS})
target_link_libraries(MappeIrpi ${ImageMagick_LIBRARIES} gdal stdc++fs)


