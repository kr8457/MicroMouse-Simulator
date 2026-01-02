# MicroMouse Simulator

A sophisticated micromouse maze simulator featuring pathfinding algorithms and interactive visualization.

---

## Table of Contents

- [Features](#features)
- [Prerequisites](#prerequisites)
- [Quick Start](#quick-start)
- [Building the Project](#building-the-project)
- [Running the Simulator](#running-the-simulator)
- [Testing](#testing)
- [Troubleshooting](#troubleshooting)

---

## Features

### Maze Generators
- **Recursive Backtracker**: A classic randomized depth-first search algorithm that creates complex, perfect mazes.
- **Eller's Algorithm**: A memory-efficient algorithm that generates mazes row-by-row, allowing for potentially infinite maze generation.

### Pathfinding Solvers
- **Breadth-First Search (BFS)**: Guarantees the shortest path in an unweighted grid.
- **Depth-First Search (DFS)**: Explores as far as possible along each branch before backtracking.
- **A* Search**: Uses heuristics (Manhattan distance) to find the most efficient path.
- **Flood Fill**: A popular algorithm for micromouse that calculates distances from the goal.
- **Wall Follower**: Implements the classic left-hand rule for maze navigation.

### Interactive UI
- Real-time visualization of generation and solving processes.
- Adjustable simulation speed.
- Manual wall toggling and maze editing.
- Zoom and pan support for large mazes.

---

## Prerequisites

Before building, ensure you have:

- **CMake** (version 3.16 or higher)
- **C++ Compiler** (GCC, Clang, MSVC, or MinGW)
- **Git** (for cloning the repository)
- **Build System** (Make, Ninja, or MSBuild)
- **Python 3** (for pre-commit hooks)

---

## Quick Start

```bash
# Clone the repository
git clone https://github.com/kr8457/MicroMouse-Simulator
cd MicroMouse-Simulator

# Switch to development branch (recommended)
git checkout dev

# Configure with testing enabled
cmake -B build -DBUILD_TESTING=ON

# Build the project
cmake --build build

# Run tests
ctest --test-dir build --output-on-failure
```

---

## Building the Project

### Basic Configuration

For a standard build without testing:
```bash
cmake -B build
```

### Configuration with Testing

To enable GoogleTest for unit testing:
```bash
cmake -B build -DBUILD_TESTING=ON
```

This will automatically fetch and build GoogleTest from source.

### Compile the Project

Build the configured project:
```bash
cmake --build build
```

---

## Running the Simulator

Run the simulator based on your system:

```bash
# For Unix-like systems
./build/mms

# For Windows (MinGW)
./build/mms.exe

# For Visual Studio builds
./build/Debug/mms.exe
```

---

## Testing

If you configured the project with `-DBUILD_TESTING=ON`, run the test suite using CTest:

```bash
ctest --test-dir build --output-on-failure
```

---

## Troubleshooting

### CMake Configuration Fails
- Verify CMake is installed: `cmake --version`
- Ensure a C++ compiler is in your PATH
- Try specifying a generator explicitly with `-G`

### Build Errors
- Check that all prerequisites are installed
- Ensure your compiler supports C++11 or higher
- Try cleaning and reconfiguring:
  ```bash
  rm -rf build/
  cmake -B build
  cmake --build build
  ```

---

## Additional Resources

- [CMake Documentation](https://cmake.org/documentation/)
- [GoogleTest GitHub](https://github.com/google/googletest)
- [Doxygen Documentation](https://www.doxygen.nl/manual/index.html)
- [Sphinx Documentation](https://www.sphinx-doc.org/)
