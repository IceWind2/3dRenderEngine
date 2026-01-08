#pragma once

#include <vector>

#include "Engine3D/RenderWindow.hpp"
#include "Engine3D/VectorMatrix.hpp"

struct UpdateData {
    float fThetaRad;
    mat4x4 matRotX, matRotZ;
};

class Engine3D {
private:
    // Window parameters
    int _width, _height, _fpsTarget;
    RenderWindow _renderWindow;
    
    // Projection matrix parameters
    float _zNear, _zFar, _fov, _aspectRatio;
    mat4x4 _matProj;
    
    // Camera parameters
    vec3d _vCameraPosition, _vCameraDirection, _vUpDirection;
    
    // Light parameters
    vec3d _lightDirection;
    
    // Scene objects
    std::vector<mesh> _objects;

    void Update(float deltaTime);
    void Render();
public:
    Engine3D(int width, int height);

    void LoadObjects();
    void StartEngineLoop();
};