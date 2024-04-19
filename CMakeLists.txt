cmake_minimum_required(VERSION 3.5)

project(CrossGui VERSION 0 LANGUAGES C) 

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -fsanitize=address")

find_package(PkgConfig REQUIRED) 
find_package(Vulkan REQUIRED) 

pkg_check_modules(CrossWindow REQUIRED crosswindow)

include_directories (Include ${CrossWindow_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIRS})

add_subdirectory(Shaders)
add_subdirectory(Source)
