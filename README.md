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
- HDR Rendering pipeline
- Dynamic resolution switching
- Dynamic fullscreen/windowed switching
- Toggleable VSync
- Screenshot support
- Runtime HDR toggle
- Runtime AA switching
- SMAA
- MSAA
- SSAA
- Combined AA modes
- Tonemapping
- Dithering (ING)
- Fullscreen triangle rendering
- Push constants
- Dynamic viewport/scissor
- Swapchain recreation
- Aspect ratio preservation
- SRGB workflow
- HDR intermediate render targets

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