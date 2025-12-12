# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

<!--
    The following heading should be used
        - Added
        - Changed
        - Deprecated
        - Removed
        - Fixed
        - Security
 -->

## [unreleased]

### Added
- Added `.clang-tidy` that implements basic `Checks` like `cppcoreguidelines`, `modernize` and `readability` etc along with basic naming-conventions for Classes, Functions and Variables.
- Added `CHANGELOG.md` to document all notable changes in a particular commit or release.
- Added `.pre-commit-config.yaml` to automate formatting, linting, analysing, spell-correction, and other pesky tasks & checklist (install using `pre-commit install`).

### Changed
- Customized `.clang-format` for C++, away from pure LLVM coding-style. See [ClangFormatStyleOptions](https://clang.llvm.org/docs/ClangFormatStyleOptions.html) for more on what each does. tl;dr, it's just better? alignment and four-space tab. pro-tip: use empty line to separate two blocks of declarations.


<!-- Here comes the `git diff` of each version -->
<!-- SEE https://github.com/nyx-4/MicroProjects/blob/main/CHANGELOG.md?plain=1 for @nyx-4's CHANGELOG -->
