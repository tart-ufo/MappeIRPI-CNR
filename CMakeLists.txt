project(MappeIrpi)
cmake_minimum_required(VERSION 3.12)
add_compile_options(-std=c++17)
set(SOURCE_FILES main.cpp)
add_executable(MappeIrpi ${SOURCE_FILES})

FIND_PACKAGE(GDAL COMPONENTS REQUIRED)
add_definitions( -DMAGICKCORE_QUANTUM_DEPTH=16 )
add_definitions( -DMAGICKCORE_HDRI_ENABLE=0 )
find_package(ImageMagick COMPONENTS Magick++)
include_directories(${ImageMagick_INCLUDE_DIRS})
target_link_libraries(MappeIrpi ${ImageMagick_LIBRARIES} gdal)


