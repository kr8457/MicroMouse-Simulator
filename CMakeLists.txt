cmake_minimum_required(VERSION 3.10)

project(MicroMouse-Simulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# --- Define Source Files ---
set(APP_SOURCES src/Main.cpp src/MazeGen.cpp)

# --- Create the Executable Target ---
add_executable(mms ${APP_SOURCES})

# --- Configure Include Directories ---

# Tell the compiler where to find header files.
# By setting this as PUBLIC, any other target that links against 'my_app' 
# will also get this include path (though typically the executable isn't linked to).
target_include_directories(mms
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include 
)
