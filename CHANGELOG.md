# Changelog

## 0.1.0 (2026-01-02)

### Added

- A SFML Based GUI
- Interactive Maze Editor
- File Explorer Save/Load
- Eller's and Recursive Backtracking algorithm for Maze Generation
- Step Generation Option.
- BFS, DFS, Wall Follower, Flood Fill and A* solving algorithms.
- Step Solve and Instant Solve options.
- Option to change Animation Speed.
- Option to change Maze Size.
- MVC Style Architecture

## [unreleased]

### Added
- Added `.clang-tidy` that implements basic `Checks` like `cppcoreguidelines`, `modernize` and `readability` etc along with basic naming-conventions for Classes, Functions and Variables.
- Added `CHANGELOG.md` to document all notable changes in a particular commit or release.
- Added `.pre-commit-config.yaml` to automate formatting, linting, analysing, spell-correction, and other pesky tasks & checklist (install using `pre-commit install`).

### Changed
- Customized `.clang-format` for C++, away from pure LLVM coding-style. See [ClangFormatStyleOptions](https://clang.llvm.org/docs/ClangFormatStyleOptions.html) for more on what each does. tl;dr, it's just better? alignment and four-space tab. pro-tip: use empty line to separate two blocks of declarations.
