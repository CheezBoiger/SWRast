# Software rasterizer test
cmake_minimum_required(VERSION 3.0)
project("SoftwareRasterizerAppTest")

set ( SW_RASTER_NAME "SoftwareRasterizerAppTest")
set ( SW_RASTER_LIB "SoftwareRasterizer")
set ( SW_RASTER_BUILD_FILES main.cpp)

set ( SW_RASTER_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Public)

add_executable(${SW_RASTER_NAME} ${SW_RASTER_BUILD_FILES})
target_include_directories(${SW_RASTER_NAME} PUBLIC ${SW_RASTER_INCLUDE_DIR})
target_link_libraries(${SW_RASTER_NAME} ${SW_RASTER_LIB})
add_dependencies(${SW_RASTER_NAME} ${SW_RASTER_LIB})
# Doing some stuff for organization.
if (MSVC)
  foreach(source IN LISTS SW_RASTER_BUILD_FILES)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
  endforeach()
endif()