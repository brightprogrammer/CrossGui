add_subdirectory(Utils)

add_executable(main Main.c) 
target_link_libraries(main crossgui_utils ${CrossWindow_LIBRARIES} ${Vulkan_LIBRARIES} m)
