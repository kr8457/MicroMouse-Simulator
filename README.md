# MicroMouse Simulator

A sophisticated micromouse maze simulator featuring pathfinding algorithms and interactive visualization.

---

## Table of Contents

- [Prerequisites](#prerequisites)
- [Quick Start](#quick-start)
- [Building the Project](#building-the-project)
  - [Step 1: Clone the Repository](#step-1-clone-the-repository)
  - [Step 2: Configure the Build](#step-2-configure-the-build)
  - [Step 3: Compile the Project](#step-3-compile-the-project)
- [Running the Simulator](#running-the-simulator)
- [Testing](#testing)
- [Troubleshooting](#troubleshooting)

---

## Prerequisites

Before building, ensure you have:

- **CMake** (version 3.16 or higher)
- **C++ Compiler** (GCC, Clang, MSVC, or MinGW)
- **Git** (for cloning the repository)
- **Build System** (Make, Ninja, or MSBuild)
- **Python 3** with **Sphinx**, **Breathe**, and **Doxygen** (for documentation)

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

### Step 1: Clone the Repository

Clone the project from GitHub:
```bash
git clone https://github.com/kr8457/MicroMouse-Simulator
```

> **Tip:** The latest features are on the `dev` branch:
> ```bash
> git checkout dev
> ```

Navigate to the project directory:
```bash
cd MicroMouse-Simulator
```

### Step 2: Configure the Build

#### Basic Configuration

For a standard build without testing:
```bash
cmake -B build
```

#### Configuration with Testing

To enable GoogleTest for unit testing:
```bash
cmake -B build -DBUILD_TESTING=ON
```

This will automatically fetch and build GoogleTest from source, integrating it with the project.

#### Choosing a Generator

You can specify different CMake generators using the `-G` flag:
```bash
# For MinGW Makefiles
cmake -B build -G "MinGW Makefiles"

# For Visual Studio 2022
cmake -B build -G "Visual Studio 17 2022"

# For Ninja
cmake -B build -G "Ninja"
```

**What are generators?** Generators produce build system configurations that tools like Make, Ninja, or MSBuild use to compile your project.

> [!NOTE]
> If no generator is specified, CMake automatically selects the default for your platform.

### Step 3: Compile the Project

Build the configured project:
```bash
cmake --build build
```

For multi-configuration generators (like Visual Studio), specify the build type:
```bash
# Release build (optimized)
cmake --build build --config Release

# Debug build (with debug symbols)
cmake --build build --config Debug
```

---

## Running the Simulator

The executable location depends on your generator and configuration:

| Generator | Executable Location |
|-----------|---------------------|
| MinGW Makefiles / Unix Makefiles | `build/mms` or `build/mms.exe` |
| Visual Studio (Debug) | `build/Debug/mms.exe` |
| Visual Studio (Release) | `build/Release/mms.exe` |
| Ninja | `build/mms` or `build/mms.exe` |

Run the simulator:
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

**Options:**
- `--output-on-failure`: Displays test output only when tests fail
- `-V` or `--verbose`: Shows detailed output for all tests
- `-R <regex>`: Run only tests matching the regular expression

### Running Specific Tests
```bash
# Run tests matching a pattern
ctest --test-dir build -R TrivialEquality --output-on-failure

# Run tests verbosely
ctest --test-dir build -V
```

## Documentation

The project uses **Doxygen** for API documentation and **Sphinx** for a structured, theme-based user guide.

### Building Documentation

1.  **Build Sphinx Docs**:
    ```bash
    cd sphinx_docs
    # Ensure dependencies: pip install sphinx breathe sphinx-rtd-theme
    doxygen Doxyfile
    make html
    ```

The generated documentation will be available in `sphinx_docs/_build/html/index.html`.

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

### Cannot Find Executable

- Check the executable location table above
- Verify the build completed without errors
- Look in `build/Debug/` or `build/Release/` for Visual Studio builds

### Test Failures

- Ensure you configured with `-DBUILD_TESTING=ON`
- Check that GoogleTest was successfully downloaded and built
- Run tests with `-V` flag for detailed output

---

## Additional Resources

- [CMake Documentation](https://cmake.org/documentation/)
- [GoogleTest GitHub](https://github.com/google/googletest)
