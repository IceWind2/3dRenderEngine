#include <algorithm>
#include <cstdio>
#include <memory>
#include <cmath>
#include <chrono>
#include <SDL3/SDL.h>

#include "Engine3D/Engine3D.hpp"
#include "Engine3D/MathTypes.hpp"
#include "SDL3/SDL_scancode.h"

Engine3D::Engine3D(int width, int height) : _width(width), _height(height) {
    // Setup rendering loop
    _renderWindow = std::make_unique<RenderWindow>("3D Graphics Engine", width, height);
    
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
    _objects.push_back(mesh("assets/VideoShip.obj"));

    _updateData = { 0.0f, {}, {} };
}

void Engine3D::StartEngineLoop() {
    SDL_Event event;
    auto previousTime = std::chrono::steady_clock::now();

    while (!_exit) {
        auto frameStart = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float, std::milli>(frameStart - previousTime).count();
        previousTime = frameStart;

        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT ||
                (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE)) {
                _exit = true;
            }
        }
        
        // Update scene
        Update(deltaTime);
        
        // Render graphics
        Render();

        // Sync frame rate to ~120 FPS
        auto frameFinish = std::chrono::steady_clock::now();
        float frameTime = std::chrono::duration<float, std::milli>(frameFinish - frameStart).count();
        if (frameTime < 8.33f) {
            SDL_Delay((Uint32)(8.33f - frameTime));
        }
    }
}

void Engine3D::Update(float deltaTime) {
    _updateData.fTheta += 0.001f * deltaTime;

    _updateData.matRotZ.m[0][0] = cosf(_updateData.fTheta);
    _updateData.matRotZ.m[0][1] = sinf(_updateData.fTheta);
    _updateData.matRotZ.m[1][0] = -sinf(_updateData.fTheta);
    _updateData.matRotZ.m[1][1] = cosf(_updateData.fTheta);
    _updateData.matRotZ.m[2][2] = 1;
    _updateData.matRotZ.m[3][3] = 1;

    _updateData.matRotX.m[0][0] = 1;
    _updateData.matRotX.m[1][1] = cosf(_updateData.fTheta);
    _updateData.matRotX.m[1][2] = sinf(_updateData.fTheta);
    _updateData.matRotX.m[2][1] = -sinf(_updateData.fTheta);
    _updateData.matRotX.m[2][2] = cosf(_updateData.fTheta);
    _updateData.matRotX.m[3][3] = 1;
}

void Engine3D::Render() {
    _renderWindow->ClearScreen();
    
    // Calculate triangles to raster
    std::vector<triangle> trianglesToRaster;
    for (auto& obj : _objects) {
        for (auto& tri : obj.tris) {
            triangle triRotatedZ, triRotatedZX, triTranslated, triProjected;
    
            MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], _updateData.matRotZ);
            MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], _updateData.matRotZ);
            MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], _updateData.matRotZ);
    
            MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], _updateData.matRotX);
            MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], _updateData.matRotX);
            MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], _updateData.matRotX);
    
            triTranslated = triRotatedZX;
            triTranslated.p[0].z += 9.0f;
            triTranslated.p[1].z += 9.0f;
            triTranslated.p[2].z += 9.0f;
            
            vec3d normal, line1, line2;
            line1 = triTranslated.p[1] - triTranslated.p[0];
            line2 = triTranslated.p[2] - triTranslated.p[0];
            CrossProduct(line1, line2, normal);
            normal.Normalize();
            
            if (DotProduct((triTranslated.p[0] - _vCamera), normal) >= 0.0f) {
                continue;
            }
            
            // Project triangles from 3D --> 2D
            MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], _matProj);
            MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], _matProj);
            MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], _matProj);
            
            triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
            triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
            triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;
            
            triProjected.p[0].x *= 0.5f * (float)_width;
            triProjected.p[0].y *= 0.5f * (float)_height;
            triProjected.p[1].x *= 0.5f * (float)_width;
            triProjected.p[1].y *= 0.5f * (float)_height;
            triProjected.p[2].x *= 0.5f * (float)_width;
            triProjected.p[2].y *= 0.5f * (float)_height;
            
            triProjected.normal = normal;
            trianglesToRaster.push_back(triProjected);
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
        float lum = DotProduct(tri.normal, _lightDirection);
        _renderWindow->DrawTriangle(tri, lum);
    }
    
    _renderWindow->Present();
}