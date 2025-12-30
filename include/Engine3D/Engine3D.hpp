#pragma once

#include <memory>
#include "Engine3D/RenderWindow.hpp"
#include "Engine3D/MathTypes.hpp"

class Engine3D {
private:
    int width, height;
    bool exit = false;
    std::unique_ptr<RenderWindow> _renderWindow;
    mesh _meshCube;
    mat4x4 _matProj;

    void MultiplyMatrixVector(const vec3d& i, vec3d& o, const mat4x4& m);
public:
    Engine3D(int width, int height);

    void Initialize();
    void StartRenderingLoop();
};