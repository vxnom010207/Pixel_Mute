# PIXEL-MUTE: Procedural Coverless Steganography

**An Exhibition Prototype for Generative Spatial Steganography via Voronoi Lattice Modulation.**

---

## Executive Summary

Traditional steganography relies on modifying the data structure of an existing carrier image (e.g., Least Significant Bit manipulation). This paradigm is inherently vulnerable to modern statistical steganalysis, which detects the mathematical anomalies left in the image's noise floor. 

**PIXEL-MUTE** proposes a paradigm shift: **Coverless Generative Steganography**. Instead of embedding a payload into a pre-existing image, the payload acts as the deterministic parameter set used to synthesize a novel image from scratch. By mapping ASCII data to spatial coordinates within a Voronoi diagram, PIXEL-MUTE generates mathematically pristine artwork where the geometry itself is the encoded data, yielding zero steganographic noise.

---

## System Architecture

The application operates on a fixed $512 \times 512$ spatial canvas, logically partitioned into an $8 \times 8$ grid matrix (yielding 64 discrete $64 \times 64$ pixel cells). This architecture supports a maximum contiguous payload of 64 bytes (characters).

### 1. The Encoding Pipeline (Synthesis)
* **Data Parsing:** The payload is processed sequentially. Each 8-bit ASCII character is split into two 4-bit sequences (nibbles).
* **Spatial Mapping:** The lower nibble determines the $X$-axis offset, and the upper nibble determines the $Y$-axis offset within the designated grid cell. A spatial padding of 8 pixels is applied to prevent cell-boundary overflow, and a scalar of 3 is applied to maximize spatial distribution.
* **Procedural Generation:** The calculated $(X, Y)$ coordinates serve as "seed" points. The engine executes a Euclidean distance algorithm across the canvas to generate a Voronoi lattice, shading pixels based on proximity to the nearest seed to produce a volumetric aesthetic.
* **Alpha Modulation (The Beacon):** To ensure deterministic spatial recovery without relying on color heuristics, the exact coordinate pixel of each seed is rendered with an Alpha channel value of `254` (out of 255). This $0.39\%$ deviation is biologically imperceptible but serves as an absolute computational beacon.

### 2. The Decoding Pipeline (Extraction)
* **Feature Extraction:** The decoder iterates through the 64 logical grid partitions, performing a linear scan for the `Alpha == 254` marker.
* **Coordinate Reversion:** Upon locating the marker, the local $(X, Y)$ coordinates are extracted.
* **Data Reconstruction:** The spatial padding and scalars are mathematically reversed to retrieve the original 4-bit nibbles, which are subsequently merged using bitwise operators to reconstruct the original 8-bit ASCII character.

---

## Technical Specifications

| Feature | Specification |
| :--- | :--- |
| **Architecture** | Coverless Synthesis (Procedural Generative) |
| **Algorithm** | Voronoi Lattice Modulation |
| **Canvas Resolution** | $512 \times 512$ pixels |
| **Grid Partitioning** | $8 \times 8$ Matrix (64 Cells, $64 \times 64$ pixels each) |
| **Maximum Payload** | 64 Bytes |
| **Marker Strategy** | Alpha Channel Delta ($\alpha=254$) |
| **Export Format** | 32-bit RGBA `.png` (Strict Lossless) |

---

## Technology Stack

* **Core Language:** C++ (Standard ISO C++11 or higher)
* **Graphics API:** [Raylib](https://www.raylib.com/) (Hardware-accelerated rendering)
* **GUI Framework:** [Raygui](https://github.com/raysan5/raygui) (Immediate-mode UI)
* **System Integration:** [tinyfiledialogs](https://sourceforge.net/projects/tinyfiledialogs/) (Native OS dialog bridging)
* **Compiler:** GCC (MinGW-w64 via MSYS2 environment)

---

## Build & Deployment Instructions (Windows)

### Prerequisites
Ensure the MSYS2 UCRT64 toolchain is installed and properly configured in your system `PATH`, along with the Raylib dependencies. 

### Compilation
Navigate to the project directory containing `main.cpp`, `tinyfiledialogs.c`, and `tinyfiledialogs.h`. Execute the following build command:

```bash
g++ .\main.cpp tinyfiledialogs.c -o PIXEL-MUTE.exe -mwindows -L. -lraylib -lopengl32 -lgdi32 -lwinmm -lcomdlg32 -lole32