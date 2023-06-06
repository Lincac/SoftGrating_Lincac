#ifndef _MAT_H_
#define _MAT_H_

#include"vec.h"

// ×Ô×óÏòÓÒ

struct mat4 {
    float data[4][4] = { {0} };

    mat4() {};
    mat4(float i) {
        data[0][0] = i;
        data[1][1] = i;
        data[2][2] = i;
        data[3][3] = i;
    };

    inline vec4 operator *(const vec4& v) const {
        vec4 temp;
        for (int i = 0; i < 4; i++) {
            temp[i] += data[i][0] * v.data[0];
            temp[i] += data[i][1] * v.data[1];
            temp[i] += data[i][2] * v.data[2];
            temp[i] += data[i][3] * v.data[3];
        }
        return temp;
    };

    inline mat4 operator *(const mat4& m) const {
        mat4 temp;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                temp[i][j] += data[i][0] * m.data[0][j];
                temp[i][j] += data[i][1] * m.data[1][j];
                temp[i][j] += data[i][2] * m.data[2][j];
                temp[i][j] += data[i][3] * m.data[3][j];
            }
        }
        return temp;
    }

    inline float* operator [](int i) {
        return data[i];
    }

    inline mat4& operator =(const mat4& m) {
        if (this != &m) {
            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++)
                    data[i][j] = m.data[i][j];
            return *this;
        }
        return *this;
    }

    friend std::ostream& operator <<(std::ostream& out, mat4& mat) {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                std::cout << mat.data[i][j] << ' ';
            }
            std::cout << '\n';
        }
        return out;
    }
};


mat4 scale(const mat4& m, vec3 v);
mat4 rotateX(const mat4& m, float angle);
mat4 rotateY(const mat4& m, float angle);
mat4 rotateZ(const mat4& m, float angle);
mat4 translate(const mat4& m, vec3 pos);

mat4 perspective(float fov, float axpect, float near, float far);
mat4 ortho(float left, float right, float bottom, float top, float znear, float zfar);
mat4 lookAt(const vec3& eye, const vec3& center, const vec3& up);

#endif