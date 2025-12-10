# 🤝 Contributing to MicroMouse Simulator

Thank you for your interest in contributing to the MicroMouse Simulator project! This guide will help you get started with contributing code, reporting issues, and following our development practices.

---

## 📋 Table of Contents

- [Getting Started](#getting-started)
- [Development Workflow](#development-workflow)
- [Coding Standards](#coding-standards)
- [Development Tools Setup](#development-tools-setup)
- [Testing Guidelines](#testing-guidelines)
- [Submitting Changes](#submitting-changes)
- [Reporting Issues](#reporting-issues)
- [Review Process](#review-process)

---

## 🚀 Getting Started

### Prerequisites

Before contributing, ensure you have:

- **Git** installed and configured
- **CMake** (version 3.16+)
- **C++ Compiler** (GCC, Clang, MSVC, or MinGW) which supports C++17 and above
- **Clang-Format** (prefarably from the LLVM toolchain)
- **Clang-Tidy** (optional but recommended)
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

5. **Keep Your Fork Synchronized**
```bash
   git pull upstream dev
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
   git pull upstream dev
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

3. **Make your changes and commit regularly:**
```bash
   git add .
   git commit -m "Add Dijkstra pathfinding algorithm"
```

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

# Update your dev branch
git checkout dev
git merge upstream/dev

# Update your feature branch with latest dev changes
git checkout your-feature-branch
git rebase dev
```

> [!IMPORTANT]
> Always run `git pull` regularly to keep your local repository synchronized with the remote.

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

Clang-Format ensures consistent code formatting across the project.

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

1. Install the **C/C++** extension by Microsoft
2. Open VSCode Settings (`Ctrl+,` or `Cmd+,`)
3. Search for "cpp format"
4. Set the following options:
   - **C_Cpp: Clang_format_path**: Path to your clang-format executable
   - **C_Cpp: Clang_format_style**: `file` (uses `.clang-format` in project root)
   - **Editor: Format On Save**: Enable this for automatic formatting

#### Manual Formatting

Format a single file:
```bash
clang-format -i src/MazeGen.cpp
```

Format all files:
```bash
#works on git bash if you are in windows
find src include -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
```

### Clang-Tidy Configuration

Clang-Tidy provides static analysis and linting for C++ code.

#### VSCode Integration

1. Install the **clangd** extension (disable C/C++ IntelliSense if using clangd)
2. Add to your VSCode `settings.json`:
```json
{
    "clangd.arguments": [
        "--compile-commands-dir=${workspaceFolder}/build",
        "--clang-tidy"
    ]
}
```

3. Generate compilation database:
```bash
cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

#### Running Clang-Tidy Manually
```bash
clang-tidy src/MazeGen.cpp -p build/
```
---

## 📤 Submitting Changes

### Commit Guidelines

Write clear, concise commit messages following the **Conventional Commits** format:
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

### Pull Request Checklist

Before submitting a PR, ensure:

- [ ] Code follows the project's naming conventions
- [ ] Code is formatted with Clang-Format
- [ ] No Clang-Tidy warnings (run static analysis)
- [ ] All tests pass locally
- [ ] New features include unit tests
- [ ] Documentation is updated (if applicable)
- [ ] Commit messages are clear and descriptive
- [ ] PR description explains what and why
- [ ] PR is targeted at the `dev` branch

### Pull Request Template

When creating a PR, include:
```markdown
## Description
Brief description of the changes

## Testing
Describe how you tested your changes

## Checklist
- [ ] Code follows style guidelines
- [ ] Tests added/updated
- [ ] Documentation updated
- [ ] No new warnings

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

## 🔍 Review Process

### What to Expect

1. **Automated Checks**: CI/CD pipeline runs tests and checks
2. **Code Review**: Maintainers review your code for quality and correctness
3. **Feedback**: You may receive comments or change requests
4. **Iteration**: Make requested changes and push updates
5. **Approval**: Once approved, your PR will be merged

### Being a Good Contributor

- **Respond to feedback** promptly and professionally
- **Ask questions** if you don't understand a review comment
- **Stay engaged** throughout the review process

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

---

## 💬 Questions?

If you have questions about contributing:

- Open a discussion on GitHub
- Check existing documentation
- Ask in your pull request

---

Thank you for contributing to MicroMouse Simulator!
