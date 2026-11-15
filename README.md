# 2D Vector Graphics Engine & Cyber Space Cat Arcade (C++ / SFML)

A modular, object-oriented **2D Graphics & Game Engine Framework** built from scratch in Modern C++17 using SFML 2.6.1. Designed with Virtual Multi-Inheritance to eliminate the C++ Diamond Problem, featuring custom pixel-level rasterization algorithms, 2D affine transformations, procedural vector typography, and an interactive space arcade showcase.

## Engine Architecture & Features

- **Virtual Multi-Inheritance Hierarchy (`GameObject`, `UpdatableObject`, `DrawableObject`, `TransformableObject`)**: Clean decoupled interfaces for rendering pipelines, delta-time logic updates, and spatial transformations.
- **Custom Rasterization Algorithms (`PrimitiveRenderer`)**:
  - **Incremental Line Drawing**: Pixel-level line computation handling shallow and steep slopes.
  - **Midpoint Circle (8-Fold Symmetry)** & **Midpoint Ellipse (4-Fold Symmetry)**: Hardware-independent curve and planetary orbit rasterization.
  - **Area Filling**: 4-connected **Boundary Fill** and **Flood Fill** algorithms.
- **2D Affine Transformations (`Point2D`, `LineSegment`)**: Real-time translation, centroid rotation, and homothety (scaling) demonstrated via a 360° rotating turret mounted on the player.
- **Procedural Vector Typography & HUD**: Custom 7-segment digital counters (`SCORE`) and vector character rendering (`HP`, `GAME OVER`, `RELOAD [R]`) drawn purely with incremental lines—zero external font files required.

## Gameplay Showcase ("Cyber Space Cat")

- **Cyber Space Cat & Mounted Turret**: Pilot a procedural vector space cat with an animated thruster flame while aiming a 360° rotating laser cannon mounted on top.
- **Collect & Survive**: Chase bouncing green energy orbs (`+20 Score`, `+5 HP`) while shooting down incoming red asteroids (`+10 Score`) that shatter into multi-colored particle explosions with camera shake.
- **Celestial Backdrop**: Features a multi-layered glowing sun with rotating rays, a ringed gas giant planet on an elliptical orbit, and an orbiting moon.

## Controls

| Key | Action |
| :--- | :--- |
| **`W` / `A` / `S` / `D`** | Move Cyber Space Cat & mounted turret to collect green energy orbs |
| **`Left` / `Right` or `Q` / `E`** | Rotate the mounted laser turret 360° around the player |
| **`Space`** | Fire laser bolts to shatter incoming red asteroids |
| **`G`** | Toggle the background engineering grid |
| **`R`** | Reload / Restart the game immediately on the Game Over screen |
| **`ESC`** | Exit application |

## Build Instructions (Windows / MinGW)

```bash
g++ -std=c++17 main.cpp Point2D.cpp LineSegment.cpp PrimitiveRenderer.cpp BitmapHandler.cpp Player.cpp -o app.exe -IC:/Libraries/SFML-2.6.1/include -LC:/Libraries/SFML-2.6.1/lib -lsfml-graphics -lsfml-window -lsfml-system