# Indigo Engine

Custom 2D/3D game engine written in C++ and Vulkan.

## Overview
This project is a custom game engine written in C++ using Vulkan.  
The goal is to build a clean, modular architecture for rendering, input, and resource management.  

The engine will be used as a foundation for future game projects.

## Goals

- Clean and simple architecture
- Vulkan-based rendering
- Separation of data and rendering logic
- Expandable for future features

## Project Structure (NEW)

```
IndigoEngine/
│
├── Engine/
│   │
│   ├── Core/
│   │   ├── Application (main loop)
│   │   ├── ApplicationDesc
│   │   ├── Window
│   │   ├── Display
│   │   └── ErrorDialog
│   │   
│   ├── Input/
│   │   ├── Input
│   │   ├── KeyCodes
│   │   └── InputMapping
│   │ 
│   └── Platform/
│       │
│       └── SDL/
│           └── SDLInput
│
└── Graphics
        Vulkan
            Core
                Instance
                Surface
                PhysicalDevice
                Device
            Command
                CommandPool
                CommandBuffer
            Sync
                Fence
                Semaphore
            Swapchain
                Swapchain
                RenderFrame
            Debug
                ValidationLayers
                DebugMessengers
                Debug
                Queues
                Swapchain
                Sync
                Validation
            
            ScenePass
                SceneRenderPass
                SceneResources
                ScenePipeline
            PostProcessPass
                SSAARenderPass
                PostRenderPass
                PostResources
            Resources
                RenderTarget
                TextureDescriptor
            Renderer
            Utils
```

## Build

Requirements:
- CMake
- Vulkan SDK
- C++ compiler (GCC/Clang)

Build steps:

```bash
mkdir build
cd build
cmake ..
make
```

## Status
Work in progress.
