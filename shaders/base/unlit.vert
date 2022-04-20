#include "common.h"

layout(location = Position) in vec3 POSITION;

#ifdef HAS_UV
    layout(location = UV_1) in vec2 TEXCOORD_0;
#endif

//----------------------------------------------------------------------------------------------------------------------
#ifdef HAS_SKIN
    layout(location = Joints_0) in vec4 JOINTS_0;
    layout(location = Weights_0) in vec4 WEIGHTS_0;

    #ifdef HAS_JOINT_TEXTURE
        layout(set = 0, binding = 2) uniform sampler2D u_jointSampler;
        layout(set = 0, binding = 3) uniform float u_jointCount;

        mat4 getJointMatrix(sampler2D smp, float index) {
            float base = index / u_jointCount;
            float hf = 0.5 / u_jointCount;
            float v = base + hf;

            vec4 m0 = texture2D(smp, vec2(0.125, v));
            vec4 m1 = texture2D(smp, vec2(0.375, v));
            vec4 m2 = texture2D(smp, vec2(0.625, v));
            vec4 m3 = texture2D(smp, vec2(0.875, v));

            return mat4(m0, m1, m2, m3);
        }
    #else
        layout(set = 0, binding = 4) uniform mat4 u_jointMatrix[JOINTS_NUM];
    #endif
#endif

//----------------------------------------------------------------------------------------------------------------------
#ifdef HAS_VERTEXCOLOR
    layout(location = Color_0) in vec4 COLOR_0;
#endif

layout(set = 0, binding = 5) uniform mat4 u_localMat;
layout(set = 0, binding = 6) uniform mat4 u_modelMat;
layout(set = 0, binding = 7) uniform mat4 u_viewMat;
layout(set = 0, binding = 8) uniform mat4 u_projMat;
layout(set = 0, binding = 9) uniform mat4 u_MVMat;
layout(set = 0, binding = 10) uniform mat4 u_MVPMat;
layout(set = 0, binding = 11) uniform mat4 u_normalMat;
layout(set = 0, binding = 12) uniform vec3 u_cameraPos;
layout(set = 0, binding = 13) uniform vec4 u_tilingOffset;

//----------------------------------------------------------------------------------------------------------------------
#ifndef OMIT_NORMAL
    #ifdef HAS_NORMAL
        layout(location = Normal) in vec3 NORMAL;
    #endif

    #ifdef HAS_TANGENT
        layout(location = Tangent) in vec4 TANGENT;
    #endif
#endif

//----------------------------------------------------------------------------------------------------------------------
#ifdef HAS_BLENDSHAPE
    #ifndef HAS_BLENDSHAPE_TEXTURE
        layout(location = 10) in vec3 POSITION_BS0;
        layout(location = 11) in vec3 POSITION_BS1;
        layout(location = 12) in vec3 POSITION_BS2;
        layout(location = 13) in vec3 POSITION_BS3;

        #ifdef HAS_BLENDSHAPE_NORMAL
            layout(location = 16) in vec3 NORMAL_BS0;
            layout(location = 17) in vec3 NORMAL_BS1;
            layout(location = 18) in vec3 NORMAL_BS2;
            layout(location = 19) in vec3 NORMAL_BS3;
        #endif

        #ifdef HAS_BLENDSHAPE_TANGENT
            layout(location = 20) in vec3 TANGENT_BS0;
            layout(location = 21) in vec3 TANGENT_BS1;
            layout(location = 22) in vec3 TANGENT_BS2;
            layout(location = 23) in vec3 TANGENT_BS3;
        #endif
    #endif
    layout(set = 0, binding = 14) uniform float u_blendShapeWeights[4];
#endif

//----------------------------------------------------------------------------------------------------------------------
layout (location = 0) out vec2 v_uv;

void main() {
    vec4 position = vec4(POSITION, 1.0);

    #ifdef HAS_BLENDSHAPE
        #ifdef HAS_BLENDSHAPE_TEXTURE

        #else
            position.xyz += POSITION_BS0 * u_blendShapeWeights[0];
            position.xyz += POSITION_BS1 * u_blendShapeWeights[1];
            position.xyz += POSITION_BS2 * u_blendShapeWeights[2];
            position.xyz += POSITION_BS3 * u_blendShapeWeights[3];

            #ifndef OMIT_NORMAL
                #if defined(HAS_NORMAL) && defined(HAS_BLENDSHAPE_NORMAL)
                    normal.xyz += NORMAL_BS0 * u_blendShapeWeights[0];
                    normal.xyz += NORMAL_BS1 * u_blendShapeWeights[1];
                    normal.xyz += NORMAL_BS2 * u_blendShapeWeights[2];
                    normal.xyz += NORMAL_BS3 * u_blendShapeWeights[3];
                #endif

                #if defined(HAS_TANGENT) && defined(NORMAL_TEXTURE) && defined(HAS_BLENDSHAPE_TANGENT)
                    tangent.xyz += TANGENT_BS0 * u_blendShapeWeights[0];
                    tangent.xyz += TANGENT_BS1 * u_blendShapeWeights[1];
                    tangent.xyz += TANGENT_BS2 * u_blendShapeWeights[2];
                    tangent.xyz += TANGENT_BS3 * u_blendShapeWeights[3];
                #endif
            #endif
        #endif
    #endif

    //------------------------------------------------------------------------------------------------------------------
    #ifdef HAS_SKIN
        #ifdef HAS_JOINT_TEXTURE
        mat4 skinMatrix =
            WEIGHTS_0.x * getJointMatrix(u_jointSampler, JOINTS_0.x) +
            WEIGHTS_0.y * getJointMatrix(u_jointSampler, JOINTS_0.y) +
            WEIGHTS_0.z * getJointMatrix(u_jointSampler, JOINTS_0.z) +
            WEIGHTS_0.w * getJointMatrix(u_jointSampler, JOINTS_0.w);
        #else
            mat4 skinMatrix =
            WEIGHTS_0.x * u_jointMatrix[int(JOINTS_0.x)] +
            WEIGHTS_0.y * u_jointMatrix[int(JOINTS_0.y)] +
            WEIGHTS_0.z * u_jointMatrix[int(JOINTS_0.z)] +
            WEIGHTS_0.w * u_jointMatrix[int(JOINTS_0.w)];
        #endif

        position = skinMatrix * position;

        #if defined(HAS_NORMAL) && !defined(OMIT_NORMAL)
            normal = vec4(skinMatrix * vec4(normal, 0.0)).xyz;
            #if defined(HAS_TANGENT) && defined(NORMAL_TEXTURE)
                tangent.xyz = vec4(skinMatrix * vec4(tangent.xyz, 0.0)).xyz;
            #endif
        #endif
    #endif

    //------------------------------------------------------------------------------------------------------------------
    #ifdef HAS_UV
        v_uv = TEXCOORD_0;
    #else
        // may need this calculate normal
        v_uv = vec2(0., 0.);
    #endif

    #ifdef NEED_TILINGOFFSET
        v_uv = v_uv * u_tilingOffset.xy + u_tilingOffset.zw;
    #endif

    //------------------------------------------------------------------------------------------------------------------
    gl_Position = u_MVPMat * position;
}