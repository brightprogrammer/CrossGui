add_subdirectory(Utils)
add_subdirectory(Plugin)

# add_executable(main Main.c) 
# target_link_libraries(main crossgui_utils ${CrossWindow_LIBRARIES} ${Vulkan_LIBRARIES} m)

add_executable(plugintest PluginTest.c) 
target_link_libraries(plugintest xui_plugin xui_utils ${CrossWindow_LIBRARIES} ${Vulkan_LIBRARIES} m)
