#include <cstdio>
#include <memory>
#include <cmath>
#include <chrono>
#include <SDL3/SDL.h>

#include "Engine3D/Engine3D.hpp"
#include "Engine3D/MathTypes.hpp"

Engine3D::Engine3D(int width, int height) : width(width), height(height) {
    _renderWindow = std::make_unique<RenderWindow>("3D Graphics Engine", width, height);

    Initialize();
}

void Engine3D::Initialize() {
    _meshCube.tris = {
        // SOUTH
        {0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f},

        // EAST
        {1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f,  1.0f, 1.0f, 1.0f},
        {1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, 1.0f},

        // NORTH
        {1.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 1.0f},
        {1.0f, 0.0f, 1.0f,  0.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f},

        // WEST
        {0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f},

        // TOP
        {0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f},
        {0.0f, 1.0f, 0.0f,   1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 0.0f},
        
        // BOTTOM
        {1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f},
    };

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
}

void Engine3D::MultiplyMatrixVector(const vec3d& i, vec3d& o, const mat4x4& m) {
    o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
    o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
    o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
    float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

    if (w != 0.0f) {
        o.x /= w; o.y /= w; o.z /= w;
    }
}

void Engine3D::StartEngineLoop() {
    SDL_Event event;
    auto previousTime = std::chrono::steady_clock::now();

    while (!exit) {
        auto frameStart = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float, std::milli>(frameStart - previousTime).count();
        previousTime = frameStart;

        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                exit = true;
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
    uData.fTheta += 0.001f * deltaTime;
}

void Engine3D::Render() {
    _renderWindow->ClearScreen();
    
    mat4x4 matRotZ, matRotX;

    matRotZ.m[0][0] = cosf(uData.fTheta);
    matRotZ.m[0][1] = sinf(uData.fTheta);
    matRotZ.m[1][0] = -sinf(uData.fTheta);
    matRotZ.m[1][1] = cosf(uData.fTheta);
    matRotZ.m[2][2] = 1;
    matRotZ.m[3][3] = 1;

    matRotX.m[0][0] = 1;
    matRotX.m[1][1] = cosf(uData.fTheta);
    matRotX.m[1][2] = sinf(uData.fTheta);
    matRotX.m[2][1] = -sinf(uData.fTheta);
    matRotX.m[2][2] = cosf(uData.fTheta);
    matRotX.m[3][3] = 1;

    for (auto tri : _meshCube.tris) {
        triangle triRotatedZ, triRotatedZX, triTranslated, triProjected;

        MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotZ);
        MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotZ);
        MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotZ);

        MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
        MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
        MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

        triTranslated = triRotatedZX;
        triTranslated.p[0].z += 3.0f;
        triTranslated.p[1].z += 3.0f;
        triTranslated.p[2].z += 3.0f;

        MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], _matProj);
        MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], _matProj);
        MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], _matProj);

        triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
        triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
        triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;

        triProjected.p[0].x *= 0.5f * (float)width;
        triProjected.p[0].y *= 0.5f * (float)height;
        triProjected.p[1].x *= 0.5f * (float)width;
        triProjected.p[1].y *= 0.5f * (float)height;
        triProjected.p[2].x *= 0.5f * (float)width;
        triProjected.p[2].y *= 0.5f * (float)height;

        _renderWindow->DrawTriangle(&triProjected);
    }

    _renderWindow->Present();
}