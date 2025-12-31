#pragma once

#include <memory>
#include "Engine3D/RenderWindow.hpp"
#include "Engine3D/MathTypes.hpp"

struct UpdateData {
    float fTheta;
};

class Engine3D {
private:
    int _width, _height;
    bool _exit = false;
    std::unique_ptr<RenderWindow> _renderWindow;
    vec3d _vCamera, _lightDirection;
    mat4x4 _matProj;
    UpdateData _uData;
    
    mesh _meshCube;

    void Update(float deltaTime);
    void Render();
public:
    Engine3D(int width, int height);

    void InitializeScene();
    void StartEngineLoop();
};