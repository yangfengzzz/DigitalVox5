#version 450

#include "base/common.h"

layout(location = Position) in vec3 POSITION;

layout(set = 0, binding = 10) uniform u_mvpNoscale {
    mat4 value;
} mvp_no_scale;

layout (location = 0) out vec3 v_cubeUV;

void main() {
    v_cubeUV = POSITION.xyz;
    gl_Position = mvp_no_scale.value * vec4(POSITION, 1.0);
    gl_Position.z = gl_Position.w;
}