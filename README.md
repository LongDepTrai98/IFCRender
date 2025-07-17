# Dragon-BIM
---

## Overview
**Dragon-BIM** is a cross-platform application for visualizing Building Information Models (IFC files) in a 3D environment. It combines a native desktop GUI with high-performance 3D rendering and an IFC parsing engine.

This viewer is designed for architects, engineers, and developers who need an interactive way to explore BIM data.

---

## Core Features
- **3D Visualization**
  - Render IFC-based models using OpenGL with the **threepp** rendering engine.
  - Smooth navigation (orbit, pan, zoom).

- **Entity Tree**
  - Hierarchical tree view for IFC entities (e.g., `IfcProject`, `IfcBuilding`, `IfcWall`).
  - Sync selection between tree and 3D scene.

- **Element Interactions**
  - **Hover Highlight**: Hover over elements to highlight them in the 3D view.
  - **Selection**: Click to select elements and inspect details.
  - **Visibility Toggle**: Hide or show any element or group.

- **Camera Tools**
  - Focus (Zoom-to) on selected elements.

---

## Architecture
- **wxWidgets**: Provides the cross-platform native GUI for menus, panels, and tree views.
- **threepp**: C++ port of Three.js for real-time 3D rendering with OpenGL.
- **Web-IFC**: A fast, lightweight IFC parser (via WebAssembly) to read BIM data and construct geometry in memory.

---

## Screenshot
<img width="1920" height="981" alt="{35B7ABE3-068C-47CA-A321-2DFBF6277296}" src="https://github.com/user-attachments/assets/7f99e06b-604e-404b-8fb2-2a9d7c7cc1ee" />
*Example: A building model loaded from IFC with entity tree and interactive 3D view.*

---

## Technology Stack
- **C++20**
- [wxWidgets](https://www.wxwidgets.org/) – GUI
- [threepp](https://github.com/markaren/threepp) – 3D Rendering
- [Web-IFC](https://github.com/tomvandig/web-ifc) – IFC Parser
- **OpenGL** + **GLAD** for rendering backend

---

## Build & Run

### Prerequisites
- C++20 compiler
- CMake 3.16+
- Installed libraries:
  - wxWidgets
  - threepp
  - GLAD
  - WebAssembly runtime for Web-IFC (e.g., Emscripten or wasm loader)

### Steps
```bash
git clone https://github.com/yourusername/bim-viewer.git
cd bim-viewer
mkdir build && cd build
cmake ..
make
./BIMViewer
```

---

## Future Features
- Material and color editing
- Measurement tools
- Section cuts and clipping planes
- Snapshot export
- Multi-format support (IFC, OBJ, GLTF)

---

## License
[MIT License](LICENSE)
