#include <algorithm>
#include <chrono>
#include <SDL3/SDL.h>

#include "Engine3D/Engine3D.hpp"
#include "Engine3D/VectorMatrix.hpp"
#include "SDL3/SDL_scancode.h"

Engine3D::Engine3D(int width, int height) : _width(width), _height(height) {
    // Setup rendering loop
    _renderWindow = std::make_unique<RenderWindow>("3D Graphics Engine", width, height);
    _fpsTarget = 120;
    
    // Setup Camera
    _vCamera = { 0.0f, 0.0f, 0.0f };
    _lightDirection = { 0.0f, 0.0f, -1.0f };
    _lightDirection.Normalize();

    // Setup Projection Matrix
    float near = 0.1f;
    float far = 1000.0f;
    float fov = 90.0f;
    float aspectRatio = (float)height / (float)width;
    float fovRad = 1.0f / tanf(fov * 0.5f * 3.14159f / 180.0f);
    _matProj.m[0][0] = aspectRatio * fovRad;
    _matProj.m[1][1] = fovRad;
    _matProj.m[2][2] = far / (far - near);
    _matProj.m[3][2] = (-far * near) / (far - near);
    _matProj.m[2][3] = 1.0f;
    _matProj.m[3][3] = 0.0f;

    InitializeScene();
}

void Engine3D::InitializeScene() {
    mesh objMesh;
    if (LoadMeshFromObjectFile("assets/VideoShip.obj", objMesh)) {
        _objects.push_back(objMesh);
    }

    _updateData = { 0.0f, {}, {} };
}

void Engine3D::StartEngineLoop() {
    SDL_Event event;
    auto previousTime = std::chrono::steady_clock::now();

    float avgFrameTime = 0.0f;
    bool exit = false;
    bool process = true;
    while (!exit) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT ||
                (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE)) {
                exit = true;
            }
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_SPACE) {
                process = !process;
                previousTime = std::chrono::steady_clock::now();
            }
        }

        if (!process) {
            SDL_Delay(5);
            continue;
        }

        // Process frame
        auto frameStart = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float, std::milli>(frameStart - previousTime).count();
        previousTime = frameStart;

        Update(deltaTime);
        
        Render();

        // Calculate frame time and FPS
        auto frameFinish = std::chrono::steady_clock::now();
        float frameTime = std::chrono::duration<float, std::milli>(frameFinish - frameStart).count();
        avgFrameTime = (avgFrameTime * 0.9f) + (frameTime * 0.1f);
        _renderWindow->SetTitle(("3D Graphics Engine - " + std::to_string((int)(1000.0f / avgFrameTime)) + " FPS").c_str());
        if (frameTime < 1000.0f / (float)_fpsTarget) {
            SDL_Delay((Uint32)(1000.0f / (float)_fpsTarget - frameTime));
        }
    }
}

void Engine3D::Update(float deltaTime) {
    _updateData.matRotZ = MatrixMakeRotationZ(_updateData.fThetaRad);
    _updateData.matRotX = MatrixMakeRotationX(_updateData.fThetaRad);
    
    _updateData.fThetaRad += 0.001f * deltaTime;
}

void Engine3D::Render() {
    _renderWindow->ClearScreen();
    
    // Calculate world matrix
    mat4x4 worldMatrix;
    worldMatrix = MatrixMultiplyMatrix(_updateData.matRotZ, _updateData.matRotX);
    worldMatrix = MatrixMultiplyMatrix(worldMatrix, MatrixMakeTranslation(0.0f, 0.0f, 9.0f));
    
    // Calculate triangles to raster
    std::vector<triangle> trianglesToRaster;
    for (auto& obj : _objects) {
        for (auto& tri : obj.tris) {
            triangle triTransformed;
            triTransformed = MatrixMultiplyTriangle(worldMatrix, tri);

            vec3d line1, line2;
            line1 = triTransformed.p[1] - triTransformed.p[0];
            line2 = triTransformed.p[2] - triTransformed.p[0];
            triTransformed.normal = line1.Cross(line2).Normalize();

            if (triTransformed.normal.Dot(triTransformed.p[0] - _vCamera) >= 0.0f) {
                continue;
            }
            
            // Project triangles from 3D --> 2D and scale into view
            triTransformed = MatrixMultiplyTriangle(_matProj, triTransformed);
    
            triTransformed.p[0] /= triTransformed.p[0].w;
            triTransformed.p[1] /= triTransformed.p[1].w;
            triTransformed.p[2] /= triTransformed.p[2].w;
            triTransformed += vec3d{1.0f, 1.0f, 0.0f};
            triTransformed.p[0].x *= 0.5f * (float)_width;
            triTransformed.p[0].y *= 0.5f * (float)_height;
            triTransformed.p[1].x *= 0.5f * (float)_width;
            triTransformed.p[1].y *= 0.5f * (float)_height;
            triTransformed.p[2].x *= 0.5f * (float)_width;
            triTransformed.p[2].y *= 0.5f * (float)_height;
            
            trianglesToRaster.push_back(triTransformed);
        }
    }
    
    // Sort triangles from back to front
    std::sort(trianglesToRaster.begin(), trianglesToRaster.end(),
              [](triangle& t1, triangle& t2) {
                  float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
                  float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
                  return z1 > z2;
              });

    // Rasterize triangles            
    for (auto& tri : trianglesToRaster) {
        float lum = tri.normal.Dot(_lightDirection);
        _renderWindow->DrawTriangle(tri, lum);
    }
    
    _renderWindow->Present();
}