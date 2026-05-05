# Debug flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

# AddressSanitizer (tylko dla Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address")

cmake_minimum_required(VERSION 3.30)
project(IndigoEngine)

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(SDL3 REQUIRED)

add_executable(IndigoEngine main.cpp
        Engine/Input/Input.h
        Engine/Input/Input.cpp
        Engine/Platform/SDL/SDLInput.h
        Engine/Platform/SDL/SDLInput.cpp
        Engine/Input/KeyCodes.h
        Engine/Input/InputMapping.h
        Engine/Input/InputMapping.cpp
        Engine/Core/Error/ErrorDialog.h
        Engine/Core/Error/ErrorDialog.cpp
        Engine/Core/Application.h
        Engine/Core/Application.cpp
        Engine/Core/Window.h
        Engine/Core/Window.cpp
        Engine/Core/Display.h
        Engine/Core/Display.cpp
        Graphics/Vulkan/VulkanSurface.h
        Graphics/Vulkan/VulkanSurface.cpp
        Graphics/Vulkan/VulkanInstance.h
        Graphics/Vulkan/VulkanInstance.cpp
        Graphics/Vulkan/VulkanPhysicalDevice.h
        Graphics/Vulkan/VulkanPhysicalDevice.cpp
        Graphics/Vulkan/VulkanDevice.h
        Graphics/Vulkan/VulkanDevice.cpp
        Graphics/Vulkan/VulkanSwapchain.h
        Graphics/Vulkan/VulkanSwapchain.cpp
        Graphics/Vulkan/Scene/VulkanSceneRenderPass.h
        Graphics/Vulkan/Scene/VulkanSceneRenderPass.cpp
        Graphics/Vulkan/VulkanRenderer.h
        Graphics/Vulkan/VulkanRenderer.cpp
        Graphics/Graphics.h
        Graphics/Graphics.cpp
        Graphics/Vulkan/VulkanCommandPool.h
        Graphics/Vulkan/VulkanCommandPool.cpp
        Graphics/Vulkan/VulkanCommandBuffers.h
        Graphics/Vulkan/VulkanCommandBuffers.cpp
        Graphics/Vulkan/VulkanDebug.h
        Graphics/Vulkan/VulkanDebug.cpp
        Graphics/Vulkan/Scene/VulkanScenePipeline.h
        Graphics/Vulkan/Scene/VulkanScenePipeline.cpp
        Engine/Core/ApplicationDesc.h
        Graphics/Vulkan/Scene/VulkanSceneResources.h
        Graphics/Vulkan/Scene/VulkanSceneResources.cpp
        Graphics/Vulkan/VulkanUtils.h
        Graphics/Vulkan/VulkanUtils.cpp
        Graphics/Vulkan/Screen/VulkanScreenRenderPass.cpp
        Graphics/Vulkan/Screen/VulkanScreenRenderPass.h
        Graphics/Vulkan/VulkanSampler.h
        Graphics/Vulkan/VulkanSampler.cpp
        Graphics/Vulkan/VulkanValidation.h
        Graphics/Vulkan/VulkanValidation.cpp
)


# include
target_include_directories(IndigoEngine PRIVATE

)

# link dirs
target_link_directories(IndigoEngine PRIVATE

)

# link libs
target_link_libraries(IndigoEngine PRIVATE
        Vulkan::Vulkan
        SDL3::SDL3
        ktx
        udev
)

# compile flags
target_compile_options(IndigoEngine PRIVATE -g -O0)


