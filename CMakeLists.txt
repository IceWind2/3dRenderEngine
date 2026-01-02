cmake_minimum_required(VERSION 3.10)
project(3dRenderEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add include and link directories
include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)

# Build sorce files into library
add_library(Engine3D SHARED
    src/Engine3D/Engine3D.cpp
    src/Engine3D/RenderWindow.cpp
    src/Engine3D/VectorMatrix.cpp
)

# Create executable
add_executable(3dRenderEngine
    src/main.cpp
)

# Link libraries
target_link_libraries(3dRenderEngine SDL3)
target_link_libraries(3dRenderEngine Engine3D)