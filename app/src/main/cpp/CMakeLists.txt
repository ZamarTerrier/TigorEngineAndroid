# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.22.1)

project("testandroid")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -DVK_USE_PLATFORM_ANDROID_KHR -DVK_NO_PROTOTYPES")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVK_USE_PLATFORM_ANDROID_KHR -DVK_NO_PROTOTYPES")

# Creates your game shared library. The name must be the same as the
# one used for loading in your Kotlin/Java or AndroidManifest.txt files.
add_library(testandroid SHARED
        main.cpp
        AndroidOut.cpp
        e_blue_print.cpp
        e_buffer.cpp
        e_camera.cpp
        e_debugger.cpp
        e_descriptor.cpp
        e_device.cpp
        e_direct.cpp
        e_math.cpp
        e_memory.cpp
        e_resource_descriptors.cpp
        e_resource_shapes.cpp
        e_shaders.cpp
        e_texture.cpp
        e_tools.cpp
        e_transform.cpp
        e_window.cpp
        game_object.cpp
        game_object_2D.cpp
        game_object_3D.cpp
        graphics_object.cpp
        light_object.cpp
        pipeline.cpp
        primitive_object.cpp
        render_texture.cpp
        shader_builder.cpp
        swapchain.cpp
        TigorEngine.cpp
        vulkan_android.cpp)

# Searches for a package provided by the game activity dependency
find_package(game-activity REQUIRED CONFIG)

# Configure libraries CMake uses to link your target library.
target_link_libraries(testandroid
        # The game activity
        game-activity::game-activity

        # EGL and other dependent libraries required for drawing
        # and interacting with Android system
        EGL
        GLESv3
        jnigraphics
        android
        log)