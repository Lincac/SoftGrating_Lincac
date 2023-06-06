#include"mat.h"

mat4 scale(const mat4& m, vec3 v) {
    mat4 temp = m;
    temp[0][0] *= v.x;
    temp[1][1] *= v.y;
    temp[2][2] *= v.z;
    return temp;
}

mat4 rotateX(const mat4& m, float angle) {
    const float c = cos(angle);
    const float s = sin(angle);

    mat4 temp;
    temp[1][1] = c; temp[1][2] = -s;
    temp[2][1] = s; temp[2][2] = c;

    return temp;
}


mat4 rotateY(const mat4& m, float angle) {
    const float c = cos(angle);
    const float s = sin(angle);

    mat4 temp;
    temp[0][0] = c; temp[0][2] = s;
    temp[2][0] = -s; temp[2][2] = c;

    return temp;
}

mat4 rotateZ(const mat4& m, float angle) {
    const float c = cos(angle);
    const float s = sin(angle);

    mat4 temp;
    temp[0][0] = c; temp[0][1] = -s;
    temp[1][0] = s; temp[1][1] = c;

    return temp;
}

mat4 translate(const mat4& m, vec3 pos) {
    mat4 temp = m;
    temp[0][3] = pos.x;
    temp[1][3] = pos.y;
    temp[2][3] = pos.z;
    return temp;
}

mat4 perspective(float fov, float aspect, float near, float far) {
    /*
    缩放,平移
    将远平面的点移动到和近平面一样
    farPos -> nearPos
    mat4 nf;
    nf[0][0] = near;
    nf[1][1] = near;
    nf[2][2] = near + far;
    nf[2][3] = -near * far;
    nf[3][2] = 1;
    */

    float a = fov / 2;

    mat4 temp;
    temp[0][0] = Math::cot(a) / aspect;
    temp[1][1] = Math::cot(a);
    temp[2][2] = -(near + far) / (far - near);
    temp[2][3] = -(2 * near * far) / (far - near);
    temp[3][2] = -1;

    return temp;
}

mat4 ortho(float left, float right, float bottom, float top, float znear, float zfar) {
    mat4 _translate(1);
    _translate[0][3] = -(left + right) / 2;
    _translate[1][3] = -(bottom + top) / 2;
    _translate[2][3] = -(zfar + znear) / 2;

    mat4 _scale(1);
    _scale[0][0] = 2 / (right - left);
    _scale[1][1] = 2 / (top - bottom);
    _scale[2][2] = 2 / (znear - zfar);

    return _scale * _translate; // 先缩放在平移
}

mat4 lookAt(const vec3& eye, const vec3& center, const vec3& up) {
    vec3 front = (eye - center);
    front.normalize();
    vec3 right = cross(up, front);
    right.normalize();
    vec3 Up = cross(front, right);
    Up.normalize();

    mat4 view = mat4(1);
    view[0][0] = right.x; view[0][1] = right.y; view[0][2] = right.z;
    view[1][0] = Up.x; view[1][1] = Up.y; view[1][2] = Up.z;
    view[2][0] = front.x; view[2][1] = front.y; view[2][2] = front.z;

    vec3 pos = vec3(-eye.x, -eye.y, -eye.z);
    view[0][3] = dot(pos, right);
    view[1][3] = dot(pos, Up);
    view[2][3] = dot(pos, front);

    return view;
}