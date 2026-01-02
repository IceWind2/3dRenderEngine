#include <algorithm>
#include <chrono>
#include <SDL3/SDL.h>
#include <cmath>
#include <cstddef>

#include "Engine3D/Engine3D.hpp"
#include "Engine3D/VectorMatrix.hpp"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_scancode.h"

Engine3D::Engine3D(int width, int height) : _width(width), _height(height) {
    // Setup rendering loop
    _renderWindow = std::make_unique<RenderWindow>("3D Graphics Engine", width, height);
    _fpsTarget = 120;
    
    // Setup camera
    _vCameraPosition = { 0.0f, 0.0f, 0.0f };
    _vCameraDirection = { 0.0f, 0.0f, 1.0f };
    _vUpDirection = { 0.0f, 1.0f, 0.0f };
    _lightDirection = { 0.0f, 0.0f, 1.0f };
    _lightDirection.Normalize();

    // Setup default projection matrix
    float near = 0.1f;
    float far = 1000.0f;
    float fov = 90.0f;
    float aspectRatio = (float)height / (float)width;
    float fovRad = 1.0f / tanf(fov * 0.5f * 3.14159f / 180.0f);
    _matProj = MatrixMakeProjection(fov, aspectRatio, near, far);

    LoadObjects();
}

void Engine3D::LoadObjects() {
    mesh objMesh;
    if (LoadMeshFromObjectFile("assets/axis.obj", objMesh)) {
        objMesh.vPosition = {0.0f, 0.0f, 10.0f};
        _objects.push_back(objMesh);
    }
}

void Engine3D::StartEngineLoop() {
    SDL_Event event;
    auto previousTime = std::chrono::steady_clock::now();

    float avgFrameTime = 0.0f;
    bool exit = false;
    bool process = true;

    while (!exit) {
        auto frameStart = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float, std::milli>(frameStart - previousTime).count();
        previousTime = frameStart;

        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT ||
                (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE)) {
                exit = true;
            }

            if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_SPACE) {
                process = !process;
            }
        }

        if (!process) {
            SDL_Delay(5);
            continue;
        }

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
    // _objects[0].radRotation.x += 0.001f * deltaTime;
    // _objects[0].radRotation.z += 0.001f * deltaTime;
    // _objects[0].radRotation.y += 0.001f * deltaTime;

    // Move camera
    float moveSpeed = 0.02f * deltaTime;
    float rotationSpeed = 0.002f * deltaTime;
    const bool* state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_W]) {
        _vCameraPosition += _vCameraDirection * moveSpeed;
    }
    if (state[SDL_SCANCODE_S]) {
        _vCameraPosition -= _vCameraDirection * moveSpeed;
    }
    if (state[SDL_SCANCODE_A]) {
        vec3d left = _vCameraDirection.Cross(_vUpDirection).Normalize();
        _vCameraPosition += left * moveSpeed;
    }
    if (state[SDL_SCANCODE_D]) {
        vec3d right = _vCameraDirection.Cross(_vUpDirection).Normalize();
        _vCameraPosition -= right * moveSpeed;
    }
    if (state[SDL_SCANCODE_Q]) {
        mat4x4 matRotY = MatrixMakeRotationY(rotationSpeed);
        _vCameraDirection = MatrixMultiplyVector(matRotY, _vCameraDirection).Normalize();
    }
    if (state[SDL_SCANCODE_E]) {
        mat4x4 matRotY = MatrixMakeRotationY(-rotationSpeed);
        _vCameraDirection = MatrixMultiplyVector(matRotY, _vCameraDirection).Normalize();
    }

    if (state[SDL_SCANCODE_LSHIFT]) {
        _vCameraPosition += _vUpDirection * moveSpeed;
    }
    if (state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]) {
        _vCameraPosition -= _vUpDirection * moveSpeed;
    }
}

void Engine3D::Render() {
    _renderWindow->ClearScreen();
    
    mat4x4 matCamera = MatrixMakeTransform(_vCameraPosition, _vCameraDirection, _vUpDirection);
    mat4x4 matView = MatrixMakeInverseTransform(matCamera);
    
    // Calculate triangles to raster
    std::vector<triangle> trianglesToRaster;
    for (auto& obj : _objects) {
        mat4x4 matRotZ = MatrixMakeRotationZ(obj.radRotation.z);
        mat4x4 matRotX = MatrixMakeRotationX(obj.radRotation.x);
        mat4x4 matRotY = MatrixMakeRotationY(obj.radRotation.y);

        mat4x4 matWorld;
        matWorld = MatrixMultiplyMatrix(matRotZ, matRotX);
        matWorld = MatrixMultiplyMatrix(matWorld, matRotY);
        matWorld = MatrixMultiplyMatrix(matWorld, MatrixMakeTranslation(obj.vPosition.x, obj.vPosition.y, obj.vPosition.z));

        for (auto& tri : obj.tris) {
            triangle triTransformed;

            // World transformation
            triTransformed = MatrixMultiplyTriangle(matWorld, tri);

            // Backface culling
            triTransformed.CalculateNormal();
            if (triTransformed.normal.Dot(triTransformed.p[0] - _vCameraPosition) >= 0.0f) {
                continue;
            }
            
            // View transformation
            triTransformed = MatrixMultiplyTriangle(matView, triTransformed);

            // Project triangles from 3D --> 2D
            triTransformed = MatrixMultiplyTriangle(_matProj, triTransformed);
    
            // Perspective divide
            triTransformed.p[0] /= triTransformed.p[0].w;
            triTransformed.p[1] /= triTransformed.p[1].w;
            triTransformed.p[2] /= triTransformed.p[2].w;

            // Viewport transform
            triTransformed.p[0].y *= -1.0f;
            triTransformed.p[1].y *= -1.0f;
            triTransformed.p[2].y *= -1.0f;
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
        float lum = -tri.normal.Dot(_vCameraDirection);
        _renderWindow->DrawTriangle(tri, lum);
    }
    
    _renderWindow->Present();
}