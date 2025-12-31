#pragma once

#include <memory>
#include "Engine3D/RenderWindow.hpp"
#include "Engine3D/MathTypes.hpp"

struct UpdateData {
    float fTheta;
};

class Engine3D {
private:
    int width, height;
    bool exit = false;
    std::unique_ptr<RenderWindow> _renderWindow;
    mat4x4 _matProj;
    mesh _meshCube;
    UpdateData uData = { 0.0f };

    void MultiplyMatrixVector(const vec3d& i, vec3d& o, const mat4x4& m);
    void Update(float deltaTime);
    void Render();
public:
    Engine3D(int width, int height);

    void Initialize();
    void StartEngineLoop();
};