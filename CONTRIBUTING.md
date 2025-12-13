# 🤝 Contributing to MicroMouse Simulator

Thank you for your interest in contributing to the MicroMouse Simulator project! This guide will help you get started with contributing code, reporting issues, and following our development practices.

---

## 📋 Table of Contents

- [Getting Started](#getting-started)
- [Development Workflow](#development-workflow)
- [Coding Standards](#coding-standards)
- [Development Tools Setup](#development-tools-setup)
- [Build System](#build-system)
- [Pre-commit Hooks](#pre-commit-hooks)
- [Testing Guidelines](#testing-guidelines)
- [Submitting Changes](#submitting-changes)
- [Reporting Issues](#reporting-issues)
- [Review Process](#review-process)

---

## 🚀 Getting Started

### Prerequisites

Before contributing, ensure you have:

- **Git** installed and configured
- **CMake** (version 3.50 to 4.2)
- **C++ Compiler** (GCC, Clang, MSVC, or MinGW) which supports C++11 and above
- **Clang-Format** (preferably from the LLVM toolchain)
- **Clang-Tidy** (for static analysis)
- **Python 3** (for pre-commit hooks)
- **VSCode** (recommended IDE)

### Setting Up Your Development Environment

1. **Fork the Repository**
   
   Click the "Fork" button on GitHub and **disable** the "Copy the main branch only" option to ensure you get the `dev` branch.

2. **Clone Your Fork**
```bash
   git clone https://github.com/YOUR_USERNAME/MicroMouse-Simulator
   cd MicroMouse-Simulator
```

3. **Add Upstream Remote**
```bash
   git remote add upstream https://github.com/kr8457/MicroMouse-Simulator
```

4. **Switch to Development Branch**
```bash
   git checkout dev
```

5. **Set Up Pre-commit Hooks**
```bash
   # Install pre-commit (if not already installed)
   pip install pre-commit
   
   # Install the git hooks
   pre-commit install
   
   # Optionally install the pre-push hook for commitizen
   pre-commit install --hook-type pre-push
```

6. **Install Commitizen**
```bash
   # Install commitizen (required for creating properly formatted commits)
   pip install commitizen
   
   # Verify installation
   cz version
```

7. **Keep Your Fork Synchronized**
```bash
   git fetch upstream
   git merge upstream/dev
```

---

## 🔄 Development Workflow

### Branching Model

We follow a **feature branch workflow** with two main branches:

- **`main`**: Production-ready code (protected, no direct pushes)
- **`dev`**: Active development branch (all features merge here first)

### Creating a Feature Branch

1. **Ensure you're on the latest `dev` branch:**
```bash
   git checkout dev
   git pull upstream/dev
```

2. **Create a descriptive feature branch:**
```bash
   # Use descriptive names that explain what you're working on
   git checkout -b feature/add-dijkstra-algorithm
   git checkout -b fix/maze-generation-bug
   git checkout -b docs/update-readme
```

   **Branch naming conventions:**
   - `feature/` - New features or enhancements
   - `fix/` - Bug fixes
   - `docs/` - Documentation updates
   - `refactor/` - Code refactoring
   - `test/` - Test additions or modifications

For additional types, visit [Conventional Branch](https://conventional-branch.github.io/).

3. **Make your changes and commit regularly:**
```bash
   # Stage your changes
   git add .
   
   # Commit using commitizen (recommended)
   # Make sure commitizen is installed: pip install commitizen
   cz commit
```

> [!NOTE]
> If you haven't installed commitizen yet, see the [Commitizen Installation](#commitizen-configuration) section.

4. **Push your branch to your fork:**
```bash
   git push -u origin feature/add-dijkstra-algorithm
```

5. **Create a Pull Request:**
   - Go to GitHub and open a PR from your branch to the `dev` branch
   - Fill out the PR with details about your changes
   - [Link](https://docs.github.com/en/issues/tracking-your-work-with-issues/using-issues/linking-a-pull-request-to-an-issue) any relevant issues.

### Syncing Your Fork

Always keep your fork up to date:
```bash
# Fetch upstream changes
git fetch upstream

# Update your local dev branch
git checkout dev
git merge upstream/dev

# Push updated dev branch to your fork
git push origin dev

# Update your feature branch with latest dev changes
git checkout your-feature-branch
git rebase dev
```

> [!IMPORTANT]
> Always run `git fetch upstream` regularly to keep your local repository synchronized with the remote.

---

## 💻 Coding Standards

### Naming Conventions

#### 1. File Names
Use **PascalCase** for all source files:
```
✅ Good:
- MazeGen.cpp / MazeGen.hpp
- PathFinder.cpp / PathFinder.hpp
- RobotController.cpp / RobotController.hpp

❌ Bad:
- maze_gen.cpp
- pathfinder.cpp
- robot-controller.cpp
```

**File Extensions:**
- `.cpp` - C++ source files
- `.hpp` - C++ header files

#### 2. Variable Names
Use **snake_case** for variables. Names should be descriptive and self-explanatory:
```cpp
✅ Good:
int cell_count;
double path_length;
bool is_visited;
std::vector<Position> current_path;

❌ Bad:
int cc;
double pl;
bool v;
std::vector<Position> cp;
```

#### 3. Class Names
Use **PascalCase** for class names:
```cpp
✅ Good:
class MazeGenerator;
class PathFinder;
class RobotController;

❌ Bad:
class maze_generator;
class pathfinder;
class robot_controller;
```

#### 4. Function Names
Use **snake_case** for function names:
```cpp
✅ Good:
void generate_maze();
int calculate_distance();
bool is_wall_present();

❌ Bad:
void generateMaze();
int calculateDistance();
bool isWallPresent();
```

#### 5. Constants and Macros
Use **UPPER_SNAKE_CASE** for constants:
```cpp
✅ Good:
const int MAX_MAZE_SIZE = 16;

❌ Bad:
const int maxMazeSize = 16;
```

---

## 🛠️ Development Tools Setup

### Clang-Format Configuration

Clang-Format ensures consistent code formatting across the project. The project uses a custom `.clang-format` configuration file based on LLVM style with modifications.

#### Installation

Install the LLVM toolchain to get Clang-Format:

**Windows:**
```bash
# Download from https://releases.llvm.org/
# Or use Chocolatey
choco install llvm
```

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get install clang-format
```

**macOS:**
```bash
brew install clang-format
```

#### VSCode Integration

The project uses **clangd** for language server functionality, which includes formatting via clang-format.

1. **Install the clangd extension** in VSCode (not the C/C++ extension)
2. **Disable the C/C++ extension** if it's installed to avoid conflicts
3. **Configure clangd** by adding to your VSCode `settings.json`:
```json
{
    "clangd.arguments": [
        "--compile-commands-dir=${workspaceFolder}/build",
        "--clang-tidy",
        "--format-style=file"
    ],
    "clangd.path": "clangd",
    "[cpp]": {
        "editor.defaultFormatter": "llvm-vs-code-extensions.vscode-clangd",
        "editor.formatOnSave": true
    },
}
```

4. **Generate compilation database** (required for clangd):
```bash
# Configure the build (compilation database is automatically generated)
cmake -B build
```

> [!NOTE]
> The `CXX_EXPORT_BUILD_COMMANDS` option is already enabled by default in the project's `CMakeLists.txt`, so the compilation database (`compile_commands.json`) will be automatically generated in the `build/` directory when you configure CMake. No additional flags are needed.

#### Manual Formatting

Format a single file:
```bash
clang-format -i src/MazeGen.cpp
```

Format all files:
```bash
# Works on git bash if you are on Windows
find src include -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
```

### Clang-Tidy Configuration

Clang-Tidy provides static analysis and linting for C++ code. The project uses a comprehensive `.clang-tidy` configuration file.

#### VSCode Integration

Clang-Tidy is integrated with clangd. See the [Clang-Format VSCode Integration](#vscode-integration) section above for setup instructions. The clangd extension will automatically use clang-tidy when configured with the `--clang-tidy` argument.

> [!IMPORTANT]
> Do not install the C/C++ extension by Microsoft when using clangd, as they conflict with each other. Only use the clangd extension.

#### Running Clang-Tidy Manually
```bash
clang-tidy src/filename.cpp -p build/
```

---

## 🔨 Build System

### CMake Configuration

The project uses CMake for building. The main `CMakeLists.txt` file defines:

- **Minimum CMake Version**: 3.50 (supports up to 4.2)
- **C++ Standard**: C++11 or above
- **Compilation Database**: Automatically exported to `build/compile_commands.json` (required for clangd)
- **Project Structure**:
  - `mms-core`: Static library containing core logic (`src/MazeGen.cpp`)
  - `mms`: Main executable (`src/Main.cpp`)
  - `mms-test`: Test executable (only built when `BUILD_TESTING=ON`)

### Building the Project

#### Basic Build
```bash
# Configure the build
cmake -B build

# Build the project
cmake --build build
```

#### Building with Tests
```bash
# Configure with testing enabled
cmake -B build -DBUILD_TESTING=ON

# Build the project
cmake --build build

# Run tests
ctest --test-dir build --output-on-failure
```

### Project Structure

```
MicroMouse-Simulator/
├── CMakeLists.txt          # Main build configuration
├── include/                 # Header files
│   └── header files (.hpp)
├── src/                     # Source files
│   ├── Main.cpp
│   └── other .cpp files
└── tests/                   # Test files
    ├── CMakeLists.txt       # Test build configuration
    └── test files (.cpp)
```

---

## 🔒 Pre-commit Hooks

The project uses pre-commit hooks to automatically check code quality before commits. Configuration is in `.pre-commit-config.yaml`.

### Setup

After cloning the repository:
```bash
# Install pre-commit (if not already installed)
pip install pre-commit

# Install the git hooks
pre-commit install

# Install pre-push hook for commitizen
pre-commit install --hook-type pre-push
```

### Hooks Overview

The pre-commit configuration includes:

#### Local Hooks (Always Run)
- **enable-testing**: Configures CMake with `BUILD_TESTING=ON`
- **build**: Builds the project using CMake
- **test**: Runs tests using CTest

#### Code Quality Hooks
- **clang-format**: Formats C++ code automatically
- **clang-tidy**: Runs static analysis on C++ code
- **cppcheck**: Performs additional static analysis on source files

#### Documentation Hooks
- **markdownlint**: Lints markdown files using `.mdl.rb` configuration
- **typos**: Checks for common typos in code and documentation

#### Commit Hooks
- **commitizen**: Validates commit messages follow Conventional Commits format
- **commitizen-branch**: Validates branch names (runs on pre-push)

#### File System Hooks
- **check-added-large-files**: Prevents files larger than 200KB
- **check-executables-have-shebangs**: Verifies executable files have shebangs
- **check-case-conflict**: Detects case sensitivity issues
- **check-illegal-windows-names**: Validates Windows filename compatibility
- **check-symlinks**: Validates symlinks
- **check-json/toml/xml/yaml**: Validates configuration file syntax
- **mixed-line-ending**: Enforces LF line endings (fixes Windows CRLF)

#### Python Hooks (if applicable)
- **check-ast**: Validates Python syntax
- **check-docstring-first**: Ensures docstrings come first
- **detect-private-key**: Prevents committing private keys
- **name-tests-test**: Validates test file naming

### Running Hooks Manually

Test all hooks:
```bash
pre-commit run --all-files
```

Test a specific hook:
```bash
pre-commit run clang-format --all-files
```

### Bypassing Hooks (Not Recommended)

If you need to bypass hooks (e.g., for WIP commits):
```bash
git commit --no-verify -m "WIP: temporary commit"
```

> [!WARNING]
> Only bypass hooks for temporary commits. All final commits should pass all hooks.

---

## 📝 Commitizen Configuration

The project uses Commitizen (configured in `cz.yaml`) to enforce consistent commit message formatting.

### Installation

Before using Commitizen, you need to install it:

```bash
# Install commitizen using pip
pip install commitizen

# Verify installation
cz version
```

**Alternative installation methods:**

**Windows (using Chocolatey):**
```bash
choco install commitizen
```

**macOS (using Homebrew):**
```bash
brew install commitizen
```

### Using Commitizen

Instead of `git commit`, use:
```bash
# Interactive commit message creation (recommended)
cz commit
```

This will guide you through creating a properly formatted commit message following the Conventional Commits specification.

### Commit Message Format

When using `git commit` directly (not recommended, but validated by pre-commit), follow the **Conventional Commits** format:
```
<type>(<scope>): <subject>

<body>

<footer>
```

**Types:**
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting, missing semicolons, etc.)
- `refactor`: Code refactoring
- `test`: Adding or updating tests
- `chore`: Maintenance tasks

**Examples:**
```bash
✅ Good:
git commit -m "feat(pathfinding): add Dijkstra algorithm implementation"
git commit -m "fix(maze): resolve wall generation bug in corner cases"
git commit -m "docs(readme): update build instructions for Windows"

❌ Bad:
git commit -m "fixed stuff"
git commit -m "updates"
git commit -m "WIP"
```

---

## 🧪 Testing Guidelines

### Prerequisites

Before running tests, ensure you have:
- Configured the build with `-DBUILD_TESTING=ON`
- Built the project successfully

### Running Tests

Tests are built and run automatically by pre-commit hooks, but you can also run them manually:

```bash
# Configure with testing enabled
cmake -B build -DBUILD_TESTING=ON

# Build the project
cmake --build build

# Run all tests
ctest --test-dir build --output-on-failure

# Run tests with verbose output
ctest --test-dir build --output-on-failure --verbose
```

### Writing Tests

- Use GoogleTest framework (automatically fetched by CMake)
- Place test files in the `tests/` directory
- Follow the naming convention: `ComponentName-Test.cpp`
- Test files are automatically discovered and registered with CTest

### Test Structure

```cpp
#include <gtest/gtest.h>
#include "MazeGen.hpp"

TEST(MazeGenTest, BasicGeneration) {
    // Your test code here
    EXPECT_TRUE(true);
}
```

---

## 📤 Submitting Changes

### Pull Request Checklist

Before submitting a PR, ensure:

- [ ] Code follows the project's naming conventions
- [ ] Code is formatted with Clang-Format (enforced by pre-commit)
- [ ] No Clang-Tidy warnings (enforced by pre-commit)
- [ ] All tests pass locally (enforced by pre-commit)
- [ ] New features include unit tests
- [ ] Documentation is updated (if applicable)
- [ ] Commit messages follow Conventional Commits format (enforced by commitizen)
- [ ] PR description explains what and why
- [ ] PR is targeted at the `dev` branch
- [ ] All pre-commit hooks pass

### Pull Request Template

When creating a PR, include:
```markdown
## Description
Brief description of the changes

## Testing
Describe how you tested your changes

## Related Issues
Closes #123
```

---

## 🐛 Reporting Issues

### Before Opening an Issue

1. **Search existing issues** to avoid duplicates
2. **Check the documentation** for known limitations
3. **Test on the latest `dev` branch** to see if it's already fixed

### Issue Template

When reporting bugs, include:
```markdown
## Bug Description
Clear description of the issue

## Steps to Reproduce
1. Step one
2. Step two
3. Step three

## Expected Behavior
What should happen

## Actual Behavior
What actually happens

## Environment
- OS: [e.g., Windows 10, Ubuntu 22.04]
- Compiler: [e.g., GCC 11.2, MSVC 2022]
- CMake Version: [e.g., 3.22]
- Branch: [e.g., dev, main]

## Screenshots
Add screenshots if applicable

## Additional Context
Any other relevant information
```

### Feature Requests

For feature requests, include:

- **Problem statement**: What problem does this solve?
- **Proposed solution**: How should it work?
- **Alternatives considered**: Other approaches you've thought about
- **Additional context**: Use cases, examples, mockups

---

## 🎯 Good First Issues

Looking for a place to start? Check out issues labeled:

- `good first issue` - Beginner-friendly tasks
- `help wanted` - Tasks needing contributors
- `documentation` - Documentation improvements

---

## 📚 Additional Resources

- [Git Workflow Guide](https://www.atlassian.com/git/tutorials/comparing-workflows/feature-branch-workflow)
- [Conventional Commits](https://www.conventionalcommits.org/)
- [GoogleTest Documentation](https://google.github.io/googletest/)
- [Clang-Format Documentation](https://clang.llvm.org/docs/ClangFormat.html)
- [Pre-commit Documentation](https://pre-commit.com/)
- [Commitizen Documentation](https://commitizen-tools.github.io/commitizen/)

---

## 💬 Questions?

If you have questions about contributing:

- Open a discussion on GitHub
- Check existing documentation
- Ask in your pull request

---

Thank you for contributing to MicroMouse Simulator!

