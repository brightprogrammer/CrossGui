file(GLOB_RECURSE VULKAN_GRAPHICS_PLUGIN_SRCS ${CMAKE_CURRENT_SOURCE_DIRECTORY} *.c)

add_library(vulkangraphics SHARED ${VULKAN_GRAPHICS_PLUGIN_SRCS})
target_link_libraries(vulkangraphics ${CrossWindow_LIBRARIES} ${Vulkan_LIBRARIES})
