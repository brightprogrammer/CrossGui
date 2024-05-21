cmake_minimum_required(VERSION 3.5)

project(CrossGui VERSION 0 LANGUAGES C) 

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -fsanitize=address")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")

# find required modules 
find_package(PkgConfig REQUIRED) 
find_package(Vulkan REQUIRED) 

# pkg-config module finds
pkg_check_modules(CrossWindow REQUIRED crosswindow)

# platfrom detection 
set(CrossGui_HOST_PLATFORM "Unknown")
set(CrossGui_HOST_PLATFROM_TYPE_ENUM "HOST_PLATFORM_TYPE_UNKNOWN")
message(STATUS "Detecting platform...")

# basic platform detection
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CrossGui_HOST_PLATFORM "Linux")
    set(CrossGui_HOST_PLATFROM_TYPE_ENUM "HOST_PLATFORM_TYPE_LINUX")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(FATAL_ERROR "Windows platform not supported!")
    set(CrossGui_HOST_PLATFROM_TYPE_ENUM "HOST_PLATFORM_TYPE_WINDOWS")
else()
    message(FATAL_ERROR "Platform not supported!")
endif()

# inform that we found a compatible platform
message(STATUS "Detected system : ${CrossGui_HOST_PLATFORM}")

# create header file to help detection of platfrom in user-code 
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Platform.h.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/Include/Anvie/Platform.h"
  @ONLY
)

include_directories (Include ${CrossWindow_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIRS})

add_subdirectory(Shaders)
add_subdirectory(Source)
