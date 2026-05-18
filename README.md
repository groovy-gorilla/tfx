# TFX

Modern remake of a 1993 flight simulator written in C++ and Vulkan.

## Overview

A modern remake/reimagination of the legendary 1993 combat flight simulator created by Digital Image Design.

This project aims to recreate the atmosphere, scale and futuristic military aesthetic of the original TFX while rebuilding the technology completely from scratch using modern Vulkan rendering.

Originally released in 1993, TFX pushed PC hardware to its limits and became famous for its massive battlefields, advanced flight simulation, dynamic weather systems, virtual cockpit and groundbreaking polygonal 3D graphics. TFX featured cutting-edge 3D graphics for 1993 and was considered one of the most technologically ambitious combat flight simulators ever created.

The original game featured:
- large-scale military operations across multiple global conflict zones
- futuristic prototype aircraft including the Eurofighter EF2000, F-22 and F-117
- dynamic lighting and weather effects
- detailed cockpit systems
- advanced flight physics and simulation mechanics
- a distinctive early-90s military flight simulator atmosphere

This remake focuses not only on modern graphics technology, but also on preserving the feeling and identity of the original game:
- the lonely atmosphere of long-range combat flights
- the cold futuristic military aesthetic
- the iconic early-90s polygonal visual style
- massive landscapes and large viewing distances
- the sensation of speed and scale that made TFX unique

The renderer is being developed entirely in C++ and Vulkan, with a strong focus on modern rendering techniques while preserving the visual identity and atmosphere of the 1993 original.

Inspired by the legendary work of Digital Image Design and the golden era of 90s combat flight simulators.

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