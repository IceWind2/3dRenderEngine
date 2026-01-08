#pragma once

#include <iostream>
#include <ostream>
#include <vector>
#include <cmath>

struct vec3d {
    float x, y, z, w;

    vec3d() : x(0), y(0), z(0), w(1) {}
    vec3d(float x, float y, float z) : x(x), y(y), z(z), w(1) {}
    vec3d(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    vec3d operator-(const vec3d& v) const {
        return { x - v.x, y - v.y, z - v.z };
    }

    vec3d operator+(const vec3d& v) const {
        return { x + v.x, y + v.y, z + v.z };
    }

    vec3d operator*(const float f) const {
        return { x * f, y * f, z * f };
    }

    vec3d operator/(const float f) const {
        if (f < 0.00001f && f > -0.00001f) {
            return vec3d{ x, y, z };
        }

        return { x / f, y / f, z / f };
    }

    vec3d& operator+=(const vec3d& v) {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }

    vec3d& operator-=(const vec3d& v) {
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }

    vec3d& operator*=(const float f) {
        x *= f; y *= f; z *= f;
        return *this;
    }

    vec3d& operator/=(const float f) {
        if (f < 0.00001f && f > -0.00001f) {
            std::cerr << "[Error] Zero division in vector divide" << std::endl;
            return *this;
        }

        x /= f; y /= f; z /= f;
        return *this;
    }

    float Length() const {
        return sqrtf(x * x + y * y + z * z);
    }

    vec3d Normal() const {
        float l = Length();
        if (l < 0.000001f && l > -0.000001f) {
            std::cerr << "[Error] Zero division in vector normalization" << std::endl;
            return *this;
        }

        return *this / l;
    }

    vec3d& Normalize() {
        float l = Length();
        if (l < 0.000001f && l > -0.000001f) {
            std::cerr << "[Error] Zero division in vector normalization" << std::endl;
            return *this;
        }

        x /= l; y /= l; z /= l;
        return *this;
    }

    float Dot(const vec3d& v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    vec3d Cross(const vec3d& v) const {
        return {
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        };
    }
};

struct triangle {
    vec3d p[3];
    vec3d normal;

    triangle() {}

    triangle(vec3d a, vec3d b, vec3d c) {
        p[0] = a;
        p[1] = b;
        p[2] = c;
    }

    triangle(const triangle& t) {
        p[0] = t.p[0];
        p[1] = t.p[1];
        p[2] = t.p[2];
        normal = t.normal;
    }

    triangle operator+(const vec3d& v) const {
        return { p[0] + v, p[1] + v, p[2] + v };
    }

    triangle operator-(const vec3d& v) const {
        return { p[0] - v, p[1] - v, p[2] - v };
    }

    triangle& operator+=(const vec3d& v) {
        p[0] += v; p[1] += v; p[2] += v;
        return *this;
    }

    triangle& operator-=(const vec3d& v) {
        p[0] -= v; p[1] -= v; p[2] -= v;
        return *this;
    }

    void CalculateNormal() {
        vec3d line1 = p[1] - p[0];
        vec3d line2 = p[2] - p[0];
        normal = line1.Cross(line2).Normalize();
    }
};

struct mesh {
    std::vector<triangle> tris;

    vec3d vPosition = {0.0f, 0.0f, 0.0f};
    vec3d radRotation = {0.0f, 0.0f, 0.0f};
};

struct mat4x4 {
    float m[4][4] = {};
};

bool LoadMeshFromObjectFile(const std::string& filename, mesh& obj);

vec3d MatrixMultiplyVector(const mat4x4& m, const vec3d& v);
triangle MatrixMultiplyTriangle(const mat4x4& m, const triangle& t);
mat4x4 MatrixMultiplyMatrix(const mat4x4& m1, const mat4x4& m2);
mat4x4 MatrixMakeIdentity();
mat4x4 MatrixMakeRotationX(float fAngleRad);
mat4x4 MatrixMakeRotationY(float fAngleRad);
mat4x4 MatrixMakeRotationZ(float fAngleRad);
mat4x4 MatrixMakeTranslation(float x, float y, float z);
mat4x4 MatrixMakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar);
mat4x4 MatrixMakeTransform(vec3d& pos, vec3d& target, vec3d& up);
mat4x4 MatrixMakeInverseTransform(mat4x4 &m);
std::vector<triangle> PlaneClipTriangle(vec3d& plane_n, const vec3d& plane_p, const triangle& in_tri);