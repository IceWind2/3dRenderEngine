#include <fstream>
#include <sstream>
#include "Engine3D/VectorMatrix.hpp"

bool LoadMeshFromObjectFile(const std::string& filename, mesh& obj) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error loading obj file " + filename << std::endl;
        return false;
    }

    std::vector<vec3d> vertices;
    while (!file.eof()) {
        char line[128];
        file.getline(line, 128);
        
        std::stringstream ss;
        ss << line;

        switch (line[0]) {
            case 'v': {
                vec3d v;
                ss.ignore((2));
                ss >> v.x >> v.y >> v.z;
                vertices.push_back(v);
                break;
            }
            case 'f': {
                int f[3];
                ss.ignore((2));
                ss >> f[0] >> f[1] >> f[2];
                obj.tris.push_back({ vertices[f[0] - 1], vertices[f[1] - 1], vertices[f[2] - 1] });
                break;
            }
        }
    }
    return true;
}

vec3d MatrixMultiplyVector(const mat4x4& m, const vec3d& v) {
    return { 
        v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + v.w * m.m[3][0],
        v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + v.w * m.m[3][1],
        v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + v.w * m.m[3][2],
        v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + v.w * m.m[3][3]
    };
}

triangle MatrixMultiplyTriangle(const mat4x4& m, const triangle& t) {
    triangle result;
    result.normal = t.normal;
    result.p[0] = MatrixMultiplyVector(m, t.p[0]);
    result.p[1] = MatrixMultiplyVector(m, t.p[1]);
    result.p[2] = MatrixMultiplyVector(m, t.p[2]);
    return result;
}

mat4x4 MatrixMultiplyMatrix(const mat4x4& m1, const mat4x4& m2) {
    mat4x4 matrix;
    for (int c = 0; c < 4; c++) {
        for (int r = 0; r < 4; r++) {
            matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
        }
    }
    return matrix;
}

mat4x4 MatrixMakeIdentity() {
    mat4x4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    return matrix;
}

mat4x4 MatrixMakeRotationX(float fAngleRad) {
    mat4x4 matrix;
    matrix.m[0][0] = 1;
    matrix.m[1][1] = cosf(fAngleRad);
    matrix.m[1][2] = sinf(fAngleRad);
    matrix.m[2][1] = -sinf(fAngleRad);
    matrix.m[2][2] = cosf(fAngleRad);
    matrix.m[3][3] = 1;
    return matrix;
}

mat4x4 MatrixMakeRotationY(float fAngleRad) {
    mat4x4 matrix;
    matrix.m[0][0] = cosf(fAngleRad);
    matrix.m[0][2] = sinf(fAngleRad);
    matrix.m[2][0] = -sinf(fAngleRad);
    matrix.m[1][1] = 1;
    matrix.m[2][2] = cosf(fAngleRad);
    matrix.m[3][3] = 1;
    return matrix;
}

mat4x4 MatrixMakeRotationZ(float fAngleRad) {
    mat4x4 matrix;
    matrix.m[0][0] = cosf(fAngleRad);
    matrix.m[0][1] = sinf(fAngleRad);
    matrix.m[1][0] = -sinf(fAngleRad);
    matrix.m[1][1] = cosf(fAngleRad);
    matrix.m[2][2] = 1;
    matrix.m[3][3] = 1;
    return matrix;
}

mat4x4 MatrixMakeTranslation(float x, float y, float z) {
    mat4x4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    matrix.m[3][0] = x;
    matrix.m[3][1] = y;
    matrix.m[3][2] = z;
    return matrix;
}