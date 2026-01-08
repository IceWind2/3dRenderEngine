#include <fstream>
#include <functional>
#include <queue>
#include <sstream>
#include <vector>

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
    triangle result(t);
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
    matrix.m[1][1] = 1;
    matrix.m[2][0] = -sinf(fAngleRad);
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

mat4x4 MatrixMakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar) {
    float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * 3.14159f);
    mat4x4 matrix;
    matrix.m[0][0] = fAspectRatio * fFovRad;
    matrix.m[1][1] = fFovRad;
    matrix.m[2][2] = (fFar + fNear) / (fFar - fNear);
    matrix.m[3][2] = -(2.0f * fFar * fNear) / (fFar - fNear);
    matrix.m[2][3] = 1.0f;
    matrix.m[3][3] = 0.0f;
    return matrix;
}

mat4x4 MatrixMakeTransform(vec3d& pos, vec3d& target, vec3d& up) {
    vec3d newForward = target.Normal();

    vec3d a = newForward * up.Dot(newForward);
    vec3d newUp = up - a;

    vec3d newRight = newUp.Cross(newForward);

    mat4x4 matrix;
    matrix.m[0][0] = newRight.x;    matrix.m[0][1] = newRight.y;    matrix.m[0][2] = newRight.z;    matrix.m[0][3] = 0.0f;
    matrix.m[1][0] = newUp.x;       matrix.m[1][1] = newUp.y;       matrix.m[1][2] = newUp.z;       matrix.m[1][3] = 0.0f;
    matrix.m[2][0] = newForward.x;  matrix.m[2][1] = newForward.y;  matrix.m[2][2] = newForward.z;  matrix.m[2][3] = 0.0f;
    matrix.m[3][0] = pos.x;         matrix.m[3][1] = pos.y;         matrix.m[3][2] = pos.z;         matrix.m[3][3] = 1.0f;
    return matrix;
}

mat4x4 MatrixMakeInverseTransform(mat4x4 &m) {
    mat4x4 matrix;
    matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.0f;
    matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.0f;
    matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.0f;
    matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
    matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
    matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
    matrix.m[3][3] = 1.0f;
    return matrix;
}

vec3d SegmentIntersectHyperplane(const vec3d& a, const vec3d& b, const std::function<float(const vec3d&)> dimSelector) {
    vec3d v = b.SubHomogeneous(a);
    float t = (dimSelector(a) - a.w) / (v.w - dimSelector(v));
    return a.AddHomogeneous(v.MulHomogeneous(t));
}

std::vector<triangle> HyperplaneClipTriangle(const triangle& in_tri, const vec3d hyperplane_n, const std::function<float(const vec3d&)> dimSelector ) {
    auto const dist = [&](const vec3d& p) {
        return hyperplane_n.DotHomogeneous(p);
    };
    
    std::vector<vec3d const*> in_points;
    std::vector<vec3d const*> out_points;

    for(const vec3d& p : in_tri.p) {
        if (dist(p) >= 0.0f) {
            in_points.push_back(&p);
        }
        else {
            out_points.push_back(&p);
        }
    }

    if (in_points.size() == 0) {
        return {};
    }
    if (in_points.size() == 3) {
        return {in_tri};
    }

    std::vector<triangle> result;

    if (in_points.size() == 1 && out_points.size() == 2) {
        triangle newTri(in_tri);
        newTri.p[0] = *in_points[0];
        newTri.p[1] = SegmentIntersectHyperplane(*in_points[0], *out_points[0], dimSelector);
        newTri.p[2] = SegmentIntersectHyperplane(*in_points[0], *out_points[1], dimSelector);

        result.push_back(newTri);
        return result;
    }

    if (in_points.size() == 2 && out_points.size() == 1) {
        triangle newTri1(in_tri);
        newTri1.p[0] = *in_points[0];
        newTri1.p[1] = *in_points[1];
        newTri1.p[2] = SegmentIntersectHyperplane(*in_points[0], *out_points[0], dimSelector);
        
        triangle newTri2(in_tri);
        newTri2.p[0] = *in_points[1];
        newTri2.p[1] = SegmentIntersectHyperplane(*in_points[1], *out_points[0], dimSelector);
        newTri2.p[2] = newTri1.p[2];

        result.push_back(newTri1);
        result.push_back(newTri2);
        return result;
    }

    std::cerr << "[Error] Invalid state in Hyperplane Clipping" << std::endl;
    return {};
}

std::queue<triangle> FrustrumClipTriangle(const triangle& in_tri) {
    std::queue<triangle> triangles;
    triangles.push(in_tri);

    std::vector<std::pair<vec3d, std::function<float(const vec3d&)>>> planes = {
        {{0, 0, -1, 1}, [](const vec3d& v) { return v.z; }},    // z = w
        {{0, 0, 1, 1}, [](const vec3d& v) { return -v.z; }},    // z = -w
        {{0, -1, 0, 1}, [](const vec3d& v) { return v.y; }},    // y = w
        {{0, 1, 0, 1}, [](const vec3d& v) { return -v.y; }},    // y = -w
        {{-1, 0, 0, 1}, [](const vec3d& v) { return v.x; }},    // x = w
        {{1, 0, 0, 1}, [](const vec3d& v) { return -v.x; }},    // x = -w
    };

    for (const auto& pair : planes) {
        int queueSize = triangles.size();
        
        for (int i = 0; i < queueSize; ++i) {
            std::vector<triangle> newTriangles = HyperplaneClipTriangle(
                triangles.front(),
                pair.first,
                pair.second);
    
            triangles.pop();
    
            for (auto& tri : newTriangles) {
                triangles.push(tri);
            }
        }
    }
    
    return triangles; 
}

vec3d VectorIntersectPlane(vec3d& plane_n, const vec3d& plane_p, const vec3d& lineStart, const vec3d& lineEnd) {
    plane_n.Normalize();
    vec3d line_v = lineEnd - lineStart;

    float D = plane_n.Dot(plane_p);
    float AN = lineStart.Dot(plane_n);
    float BN = lineEnd.Dot(plane_n);
    float t = (D - AN) / (BN - AN);
    
    return lineStart + line_v * t;
}

std::vector<triangle> PlaneClipTriangle(vec3d& plane_n, const vec3d& plane_p, const triangle& in_tri) {
    plane_n.Normalize();

    auto const dist = [&](const vec3d& p)  {
        return plane_n.Dot(p - plane_p);
    };

    std::vector<vec3d const*> in_points;
    std::vector<vec3d const*> out_points;

    for(auto& p : in_tri.p) {
        if (dist(p) >= 0.0f) {
            in_points.push_back(&p);
        }
        else {
            out_points.push_back(&p);
        }
    }

    if (in_points.size() == 0) {
        return {};
    }
    if (in_points.size() == 3) {
        return {in_tri};
    }

    std::vector<triangle> result;

    if (in_points.size() == 1 && out_points.size() == 2) {
        triangle newTri(in_tri);
        newTri.p[0] = *in_points[0];
        newTri.p[1] = VectorIntersectPlane(plane_n, plane_p, *in_points[0], *out_points[0]);
        newTri.p[2] = VectorIntersectPlane(plane_n, plane_p, *in_points[0], *out_points[1]);

        result.push_back(newTri);
        return result;
    }

    if (in_points.size() == 2 && out_points.size() == 1) {
        triangle newTri1(in_tri);
        newTri1.p[0] = *in_points[0];
        newTri1.p[1] = *in_points[1];
        newTri1.p[2] = VectorIntersectPlane(plane_n, plane_p, *in_points[0], *out_points[0]);
        
        triangle newTri2(in_tri);
        newTri2.p[0] = *in_points[1];
        newTri2.p[1] = VectorIntersectPlane(plane_n, plane_p, *in_points[1], *out_points[0]);
        newTri2.p[2] = newTri1.p[2];

        result.push_back(newTri1);
        result.push_back(newTri2);
        return result;
    }

    return result;
}