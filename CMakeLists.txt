cmake_minimum_required(VERSION 3.0)
project(3dRenderEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add include and link directories
include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)

# Create executable from all source files
add_executable(3dRenderEngine
    src/main.cpp
    src/render/Frame.cpp
    src/render/RenderEngine.cpp
    src/render/RenderWindow.cpp
)

# Link libraries
target_link_libraries(3dRenderEngine SDL3)