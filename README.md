# TFX

Modern remake of a 1993 flight simulator written in C++ and Vulkan.

## Overview

TFX is a custom flight simulator project built from scratch using:
- C++20
- Vulkan
- SDL3

The project focuses on modern rendering architecture, clean code structure, and low-level graphics programming.

---

## Features

- Vulkan renderer
- SDL3 platform layer
- Multi-pass rendering
- Post-processing pipeline
- Custom rendering architecture

---

## Project Structure

```text
TFX/
│
├── Engine/
│   │
│   ├── Core/
│   │   │
│   │   ├── Application/
│   │   ├── Window/
│   │   └── Display/
│   │
│   ├── Input/
│   │
│   ├── Platform/
│   │   │
│   │   └── SDL/
│   │
│   ├── Debug/
│   │
│   ├── Graphics/
│   │   │
│   │   ├── Renderer
│   │   │
│   │   ├── Passes
│   │   │   │
│   │   │   ├── Scene
│   │   │   └── PostProcess
│   │   │
│   │   ├── Resources
│   │   │
│   │   └── Vulkan
│   │       │
│   │       ├── Core
│   │       ├── Command
│   │       ├── Swapchain
│   │       ├── Sync
│   │       ├── Debug
│   │       └── Resources
│   │
│   └── pch.h
│
├── Sandbox/
│
├── Game/
│
├── Assets/
│   │
│   └── Shaders/
│
├── ThirdParty/
│
└── CMake/
```

---

## Build Requirements

- CMake
- Vulkan SDK
- C++20 compatible compiler
- SDL3

---

## Build

```bash
mkdir build
cd build

cmake ..
make
```

---

## Status

Work in progress.