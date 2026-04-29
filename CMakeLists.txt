# Debug flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

# AddressSanitizer (tylko dla Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address")

cmake_minimum_required(VERSION 3.30)
project(TFX)

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(SDL3 REQUIRED)

# GLFW
pkg_check_modules(GLFW REQUIRED glfw3)

add_executable(TFX main.cpp
        System.h
        System.cpp
        Engine/Input/Input.h
        Engine/Input/Input.cpp
        Graphics.h
        Graphics.cpp
        Vulkan.h
        Vulkan.cpp
        Settings.h
        ErrorDialog.h
        ErrorDialog.cpp
        Font.h
        Font.cpp
        Texture.h
        Texture.cpp
        Bitmap.h
        Bitmap.cpp
        BitmapRenderer.h
        BitmapRenderer.cpp
        Renderer.h
        Renderer.cpp
        Platform/SDL/SDLInput.h
        Platform/SDL/SDLInput.cpp
        Engine/Input/KeyCodes.h
        Engine/Input/InputMapping.h
        Engine/Input/InputMapping.cpp)


# include
target_include_directories(TFX PRIVATE

)

# link dirs
target_link_directories(TFX PRIVATE

)

# link libs
target_link_libraries(TFX PRIVATE
        Vulkan::Vulkan
        SDL3::SDL3
        ktx
        udev
)

# compile flags
target_compile_options(TFX PRIVATE -g -O0)


