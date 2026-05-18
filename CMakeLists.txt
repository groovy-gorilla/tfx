# Debug flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

# AddressSanitizer (tylko dla Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address")

cmake_minimum_required(VERSION 3.30)
project(TFX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(EXECUTABLE_NAME TFX)

find_package(Vulkan REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(SDL3 REQUIRED)

add_executable(TFX main.cpp
        Engine/Input/Input.h
        Engine/Input/Input.cpp
        Engine/Platform/SDL/SDLInput.h
        Engine/Platform/SDL/SDLInput.cpp
        Engine/Input/KeyCodes.h
        Engine/Input/InputMapping.h
        Engine/Input/InputMapping.cpp
        Engine/Debug/ErrorDialog.h
        Engine/Debug/ErrorDialog.cpp
        Engine/Core/Application.h
        Engine/Core/Application.cpp
        Engine/Core/Window.h
        Engine/Core/Window.cpp
        Engine/Core/Display.h
        Engine/Core/Display.cpp
        Engine/Graphics/Vulkan/Core/VulkanSurface.h
        Engine/Graphics/Vulkan/Core/VulkanSurface.cpp
        Engine/Graphics/Vulkan/Core/VulkanInstance.h
        Engine/Graphics/Vulkan/Core/VulkanInstance.cpp
        Engine/Graphics/Vulkan/Core/VulkanPhysicalDevice.h
        Engine/Graphics/Vulkan/Core/VulkanPhysicalDevice.cpp
        Engine/Graphics/Vulkan/Core/VulkanDevice.h
        Engine/Graphics/Vulkan/Core/VulkanDevice.cpp
        Engine/Graphics/Vulkan/Core/VulkanSwapchain.h
        Engine/Graphics/Vulkan/Core/VulkanSwapchain.cpp
        Engine/Graphics/Vulkan/ScenePass/VulkanSceneRenderPass.h
        Engine/Graphics/Vulkan/ScenePass/VulkanSceneRenderPass.cpp
        Engine/Graphics/Vulkan/VulkanRenderer.h
        Engine/Graphics/Vulkan/VulkanRenderer.cpp
        Engine/Graphics/Graphics.h
        Engine/Graphics/Graphics.cpp
        Engine/Graphics/Vulkan/Core/VulkanDebug.h
        Engine/Graphics/Vulkan/Core/VulkanDebug.cpp
        Engine/Graphics/Vulkan/ScenePass/VulkanScenePipeline.h
        Engine/Graphics/Vulkan/ScenePass/VulkanScenePipeline.cpp
        Engine/Core/ApplicationDesc.h
        Engine/Graphics/Vulkan/ScenePass/VulkanSceneResources.h
        Engine/Graphics/Vulkan/ScenePass/VulkanSceneResources.cpp
        Engine/Graphics/Vulkan/Utils/VulkanUtils.h
        Engine/Graphics/Vulkan/Utils/VulkanUtils.cpp
        Engine/Graphics/Vulkan/PostProcessPass/VulkanPostRenderPass.cpp
        Engine/Graphics/Vulkan/PostProcessPass/VulkanPostRenderPass.h
        Engine/Graphics/Vulkan/Core/VulkanValidation.h
        Engine/Graphics/Vulkan/Core/VulkanValidation.cpp
        Engine/Graphics/Vulkan/Resources/RenderTarget.h
        Engine/Graphics/Vulkan/PostProcessPass/VulkanPostResources.h
        Engine/Graphics/Vulkan/PostProcessPass/VulkanPostResources.cpp
        Engine/Graphics/Vulkan/Core/VulkanCommands.h
        Engine/Graphics/Vulkan/Core/VulkanCommands.cpp
        Engine/Graphics/Vulkan/Core/VulkanSync.h
        Engine/Graphics/Vulkan/Core/VulkanSync.cpp
        Engine/Graphics/Vulkan/Core/VulkanQueues.h
        Engine/Graphics/Vulkan/Core/VulkanQueues.cpp
        Engine/Graphics/Vulkan/Resources/VulkanTextureDescriptor.h
        Engine/Graphics/Vulkan/Resources/VulkanTextureDescriptor.cpp
        Engine/Graphics/Vulkan/PostProcessPass/VulkanSSAARenderPass.h
        Engine/Graphics/Vulkan/PostProcessPass/VulkanSSAARenderPass.cpp
        ThirdParty/stb/stb_image_write.h
        Engine/pch.h
        Engine/pch.cpp
        Engine/Graphics/Vulkan/PostProcessPass/VulkanSMAARenderPass.cpp
        Engine/Graphics/Vulkan/PostProcessPass/VulkanSMAARenderPass.h
        Engine/Graphics/Vulkan/Resources/RenderTarget.cpp
        Engine/Core/Timer.h
        Engine/Core/Timer.cpp
)


# include
target_include_directories(TFX PRIVATE
    ${CMAKE_SOURCE_DIR}/Engine
    ${CMAKE_SOURCE_DIR}/
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

target_precompile_headers(TFX PRIVATE
    Engine/pch.h
)

# compile flags
target_compile_options(TFX PRIVATE -g -O0)


