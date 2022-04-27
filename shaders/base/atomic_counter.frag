#version 450

layout(location = 0) in vec2 v_uv;
layout(location = 0) out vec4 o_color;
layout(set = 0, binding = 6) uniform atomic_uint atomic_counter;

void main() {
    uint atomic = atomicCounter(atomic_counter) % 255;
    o_color = vec4(atomic/255.0, 1 - atomic/255.0, atomic/255.0, 1.0);
}
