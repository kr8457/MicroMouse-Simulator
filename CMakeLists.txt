# Specifies the minimum version of CMake required to process this file.
# Using a range (3.50..4.0) ensures compatibility with modern features.
cmake_minimum_required(VERSION 3.50..4.2)

# Defines the project name and enables the C++ compiler.
project(MicroMouse-Simulator LANGUAGES CXX)

# Sets the C++ standard for the entire project to C++17.
# This ensures consistency across all source files and compilers.
set(CMAKE_CXX_STANDARD 17)

set(CXX_EXPORT_BUILD_COMMANDS ON)

# --- SFML Integration using FetchContent (Source Code) ---
include(FetchContent)
FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.1
)

# Configure SFML options to build only what we need, statically
set(SFML_BUILD_WINDOW ON CACHE BOOL "" FORCE)
set(SFML_BUILD_GRAPHICS ON CACHE BOOL "" FORCE)
set(SFML_BUILD_SYSTEM ON CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE) # Force static linking

FetchContent_MakeAvailable(sfml)

# Note: FetchContent_MakeAvailable(sfml) provides the targets SFML::Graphics, SFML::Window, etc.
# directly, so find_package is not needed. The targets will handle include directories automatically.

# --- GoogleTest Integration (Conditional) ---

# This block executes only if the user configures CMake with -DBUILD_TESTING=ON.
if(BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        # Use a specific stable commit or tag (like release-1.14.0) for reliability.
        GIT_TAG        52eb8108c5bdec04579160ae17225d66034bd723
        # Recommended: Add DOWNLOAD_EXTRACT_TIMESTAMP TRUE for build robustness (CMP0135).
    )

    # Makes the googletest targets (GTest::gtest, GTest::gtest_main, etc.) available 
    # for linking in subsequent targets (like mms-test).
    FetchContent_MakeAvailable(googletest)

    # Include the CTest module for test registration and management.
    include(CTest)

    # Add the 'tests' subdirectory, which contains its own CMakeLists.txt file.
    add_subdirectory(tests)
endif()

# --- Core Logic Library Definition ---

# Define the source files that contain the reusable application logic.
set(CORE_SOURCES src/MazeGen.cpp src/MazeSolver.cpp)

# Create a Static Library target. This bundles the core logic into a reusable 
# library that can be linked by both the main executable and the test executable.
add_library(mms-core STATIC ${CORE_SOURCES})

# Configure Include Directories for mms-core
# Set the project's header directory as PUBLIC for the 'mms-core' library.
target_include_directories(mms-core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include 
)
# The PUBLIC keyword ensures that any target that links to 'mms-core' (like 'mms' 
# and 'mms-test') automatically inherits this include path.

# --- Main Executable Definition ---

# Define the source file(s) for the application entry point.
set(APP_SOURCES src/Main.cpp)

# Create the main executable target
add_executable(mms ${APP_SOURCES})

# Link the core logic library and SFML to the main executable.
# PRIVATE linkage means the executable uses the library internally, 
# but downstream targets don't need to know about it (standard best practice).
# Link the core logic library and SFML to the main executable.
# PRIVATE linkage means the executable uses the library internally, 
# but downstream targets don't need to know about it (standard best practice).
target_link_libraries(mms PRIVATE mms-core sfml-graphics sfml-window sfml-system)

# Define SFML_STATIC to ensure headers use static linkage conventions
target_compile_definitions(mms PRIVATE SFML_STATIC)


