project(MappeIrpi)
cmake_minimum_required(VERSION 3.12)

add_compile_options(-std=c++11)

set(
        SOURCE_FILES
        main.cpp
)
add_executable(MappeIrpi ${SOURCE_FILES})
target_link_libraries(MappeIrpi gdal)