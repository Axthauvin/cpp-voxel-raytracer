# C++ Minecraft Terrain Renderer

> A custom C++ voxel engine built from scratch using Ray Tracing and Digital Differential Analyzer (DDA) algorithms for procedural terrain generation, with the ability to load and render Minecraft schematics.

**Made by Axel Thauvin and Johan Emmanuelli**

---

## Overview

This project is a CPU-based rendering engine designed to generate voxel environments.

![Render Screenshot](minecraft_terrain.png)

We can also load Minecraft schematics directly into the engine, allowing us to render custom structures and landscapes created in Minecraft.

![Render schematic](schematic_dragongamer.schem.png)

## Features

- **Procedural Generation**: The engine uses Perlin Noise and Fractal Brownian Motion (FBM) to create realistic and varied terrain features such as hills, valleys, and water lakes.
- **DDA Ray Tracing**: Implements the Digital Differential Analyzer algorithm for fast and efficient ray traversal through dense 3D voxel grids.
- **Stolen Textures (oops)**: Integrates decompiled block textures directly from Minecraft 1.21.1
- **Multi-threading**: Using OpenMP to distribute ray calculations across multiple CPU cores.

---

## Installation and Usage

## Before you start

Make sure you have glfw3 installed on your system, as it is required for window management and rendering. You can install it using your package manager or from the official website.

On Debian-based systems, you can install it using:

```bash
sudo apt-get install libglfw3-dev
```

### Build

To compile the source code and prepare the test environment, run the following command in the project root:

```bash
make test
```

### Run

Once compiled, execute the generated binary to initiate the rendering process:

```bash
./test
```
