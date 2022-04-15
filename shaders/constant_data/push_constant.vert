#version 320 es
//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord_0;
layout(location = 2) in vec3 normal;

layout(push_constant, std430) uniform MVPUniform
{
	mat4 model;
	mat4 view_proj;

#ifdef PUSH_CONSTANT_LIMIT_256
	mat4 scale;
	mat4 padding;
#endif
} mvp_uniform;

layout (location = 0) out vec4 o_pos;
layout (location = 1) out vec2 o_uv;
layout (location = 2) out vec3 o_normal;

void main(void)
{
#ifdef PUSH_CONSTANT_LIMIT_256
    o_pos = mvp_uniform.model * mvp_uniform.scale * vec4(position, 1.0);
#else
    o_pos = mvp_uniform.model * vec4(position, 1.0);
#endif

    o_uv = texcoord_0;

    o_normal = mat3(mvp_uniform.model) * normal;

    gl_Position = mvp_uniform.view_proj * o_pos;
}
