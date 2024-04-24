add_subdirectory(Graphics)


if(CrossGui_HOST_PLATFORM STREQUAL "Linux")
    set(LOADER_API_LIBS "dl")
else()
    message(FATAL_ERROR "Platform not detected")
endif()

add_library(xui_plugin SHARED Plugin.c)
target_link_libraries(xui_plugin ${LOADER_API_LIBS})
