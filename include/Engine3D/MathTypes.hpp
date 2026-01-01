#pragma once

#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <string>

struct vec3d {
    float x, y, z;

    inline vec3d operator-(const vec3d& v) const {
        return vec3d{ x - v.x, y - v.y, z - v.z };
    }

    inline vec3d operator+(const vec3d& v) const {
        return vec3d{ x + v.x, y + v.y, z + v.z };
    }

    inline void operator+=(const vec3d& v) {
        x += v.x; y += v.y; z += v.z;
    }
    
    void Normalize() {
        float length = sqrtf(x * x + y * y + z * z);
        if (length != 0.0f) {
            x /= length;
            y /= length;
            z /= length;
        }
    }
};

struct triangle {
    vec3d p[3];
    vec3d normal;
};

struct mesh {
    std::vector<triangle> tris;

    mesh() = default;
    mesh(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error loading obj file " + filename << std::endl;
            return;
        }

        std::vector<vec3d> vertices;
        while (!file.eof()) {
            char line[128];
            file.getline(line, 128);
            
            std::stringstream ss;
            ss << line;

            switch (line[0]) {
                case 'v':
                    vec3d v;
                    ss.ignore((2));
                    ss >> v.x >> v.y >> v.z;
                    vertices.push_back(v);
                    break;
                case 'f':
                    int f[3];
                    ss.ignore((2));
                    ss >> f[0] >> f[1] >> f[2];
                    tris.push_back({ vertices[f[0] - 1], vertices[f[1] - 1], vertices[f[2] - 1] });
                    break;
            }
        }
    }
};

struct mat4x4 {
    float m[4][4] = { 0 };
};

void inline MultiplyMatrixVector(const vec3d& i, vec3d& o, const mat4x4& m) {
    o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
    o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
    o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
    float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

    if (w != 0.0f) {
        o.x /= w; o.y /= w; o.z /= w;
    }
}

float inline DotProduct(const vec3d& a, const vec3d& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

void inline CrossProduct(const vec3d& a, const vec3d& b, vec3d& result) {
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
}