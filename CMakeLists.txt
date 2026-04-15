cmake_minimum_required(VERSION 3.5)
project(vulkan)

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

add_executable(vulkan_app main.cpp
        SystemClass.h
        SystemClass.cpp
        InputClass.h
        InputClass.cpp
        GraphicsClass.h
        GraphicsClass.cpp
        VulkanClass.h
        VulkanClass.cpp
        Settings.h)

target_include_directories(vulkan_app PRIVATE ${GLFW_INCLUDE_DIRS})
target_link_directories(vulkan_app PRIVATE ${GLFW_LIBRARY_DIRS})

target_link_libraries(vulkan_app
        Vulkan::Vulkan
        ${GLFW_LIBRARIES}
)