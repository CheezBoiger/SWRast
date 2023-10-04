# Software Rasterizer
cmake_minimum_required(VERSION 3.0)
project("SoftwareRasterizer")

set ( SW_RASTER_NAME "SoftwareRasterizer")

set ( SW_RASTER_BUILD_FILES )

set ( SW_RASTER_INCLUDE_DIR Public/SoftwareRaster)
set ( SW_RASTER_SOURCE_DIR Private )

include ( CMake/RasterLib.cmake )

add_library(${SW_RASTER_NAME} SHARED ${SW_RASTER_BUILD_FILES} )
target_include_directories(${SW_RASTER_NAME} PUBLIC ${SW_RASTER_INCLUDE_DIR})

# Doing some stuff for organization.
if (MSVC)
  foreach(source IN LISTS SW_RASTER_BUILD_FILES)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
  endforeach()
endif()

add_subdirectory(App)