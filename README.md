# FBX_Renderer
A lightweight 3D asset visualizer built with C++17 and OpenGL 3.3 Core Profile. The application provides an interactive viewport to load and inspect .fbx models using a clean, modern OpenGL pipeline coupled with a native file dialog and ImGUI.


## Navigation
* **Open Model:** Click the Open File button inside the ImGui overlay and select any valid .fbx file.

 
* **Rotate Model:** Hold Left Mouse Button (LMB) and drag to orbit around the center structure.

 
* **Zoom:** Use the Mouse Scroll Wheel to adjust camera distance.


* **Exit:** Press the ESCAPE key at any point to terminate processes safely.
  

  <img width="1917" height="1150" alt="image" src="https://github.com/user-attachments/assets/7da2d57e-3726-4626-be6f-97f9c8b3c669" />

## Architecture

The project splits functionality into object-oriented layout inside the src/ directory:

* **main.cpp:** Manages the GLFW window life cycle, coordinates frame loops, initializes the OpenGL state machine (GLAD), compiles GLSL shaders, and drives ImGui rendering data.


* **Mesh (mesh.h/mesh.cpp):** Leverages Assimp to parse node hierarchies recursively. It abstracts GPU buffer configuration by mapping vertices to unique Vertex Array Objects (VAO), Vertex Buffer Objects (VBO), and Element Buffer Objects (EBO) lists.


* **Camera (Camera.h/Camera.cpp):** An implementation of an orbital view matrix converter. Uses basic spherical coordinates to calculate camera positional vectors dynamically based on yaw and pitch controls.

## Dependencies

| Dependency | Version | Purpose | 
| :--- | :---: | :--- |
| **GLFW** | 3.4 | Window management & inputs |
| **GLM** | 1.0.1 | Vector and Matrix Maths |
| **GLAD** | Local Vendor | OpenGL loader |
| **Dear ImGui** | Remote Fetch | UI overlay|
| **NFD** | Remote Fetch | file dialog execution |
| **Assimp** | 5.3.1 | 3D Asset importing |

## Project Setup

```text
└── FBX_Renderer/
    ├── CMakeLists.txt
    ├── external/
    │   └── glad/
    │       ├── include/
    │       └── src/
    │           └── glad.c
    ├── shaders/
    │   ├── vert.shader
    │   └── frag.shader
    └── src/
        ├── Camera.cpp
        ├── Camera.h
        ├── main.cpp
        ├── mesh.cpp
        └── mesh.h
```

## Build Instructions

Run the following commands from your project root:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```
Once compilation completes, find the executable binary generated in the root folder and run it.
