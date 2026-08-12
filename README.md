# Marching Cubes Renderer
Renders procedural terrain using marching cubes.

Currently samples 2D and 3D noise to generate a landscape (it performs exceedingly badly).


## Controls
WASD, LEFT SHIFT and SPACE to move - exactly like Minecraft creative.
Mouse to look around.

## Build and run instructions
1. **Generate build files and get dependencies:**
    ```bash
    cmake -B build
    ```
2. **Compile and run:**
    ```bash
    cmake --build build --target run
    ```


## License
This programme is licensed under the [MIT License](LICENSE).
