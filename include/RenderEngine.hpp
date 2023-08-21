#pragma once

#include <iostream>

#include "RenderWindow.hpp"

class RenderEngine {
private:
    RenderWindow _rWindow;
    bool _engineRunning;

public:
    RenderEngine() : _rWindow("Test window", 1280, 720), _engineRunning(false) {
        std::cout << "Start engine" << std::endl;
    }

    void Start();

    ~RenderEngine() {
        std::cout << "Stop engine" << std::endl;
    };
};