#pragma once

#include <memory>
#include <vector>
#include "Engine3D/RenderWindow.hpp"
#include "Engine3D/VectorMatrix.hpp"

struct UpdateData {
    float fThetaRad;
    mat4x4 matRotX, matRotZ;
};

class Engine3D {
private:
    int _width, _height, _fpsTarget;
    std::unique_ptr<RenderWindow> _renderWindow;
    vec3d _vCameraPosition, _vCameraDirection, _vUpDirection, _lightDirection;
    mat4x4 _matProj;
    
    std::vector<mesh> _objects;

    void Update(float deltaTime);
    void Render();
public:
    Engine3D(int width, int height);

    void LoadObjects();
    void StartEngineLoop();
};