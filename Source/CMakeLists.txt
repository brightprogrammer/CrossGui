add_subdirectory(Utils)
add_subdirectory(Plugin)

add_executable(main Main.c) 
target_link_libraries(main xui_utils xui_plugin ${CrossWindow_LIBRARIES} ${Vulkan_LIBRARIES} m)
