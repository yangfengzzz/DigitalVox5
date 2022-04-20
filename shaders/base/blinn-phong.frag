#define PI 3.14159265359
#define RECIPROCAL_PI 0.31830988618
#define EPSILON 1e-6
#define LOG2 1.442695

#define saturate(a) clamp(a, 0.0, 1.0)
#define whiteCompliment(a) (1.0 - saturate(a))

vec4 RGBMToLinear(vec4 value, float maxRange) {
    return vec4(value.rgb * value.a * maxRange, 1.0);
}

vec4 gammaToLinear(vec4 srgbIn){
    return vec4(pow(srgbIn.rgb, vec3(2.2)), srgbIn.a);
}

vec4 linearToGamma(vec4 linearIn){
    return vec4(pow(linearIn.rgb, vec3(1.0 / 2.2)), linearIn.a);
}

layout(set = 0, binding = 5) uniform cameraData {
    mat4 view_mat;
    mat4 proj_mat;
    mat4 vp_mat;
    mat4 view_inv_mat;
    mat4 proj_inv_mat;
    vec3 camera_pos;
} camera_data;

layout(set = 0, binding = 6) uniform rendererData {
    mat4 local_mat;
    mat4 model_mat;
    mat4 normal_mat;
} renderer_data;

varying vec2 v_uv;

//----------------------------------------------------------------------------------------------------------------------
#ifdef HAS_NORMAL
    #if defined(HAS_TANGENT) && defined(NORMAL_TEXTURE)
        varying mat3 v_TBN;
    #else
        varying vec3 v_normal;
    #endif
#endif

//----------------------------------------------------------------------------------------------------------------------
#ifdef HAS_VERTEXCOLOR
    varying vec4 v_color;
#endif

//----------------------------------------------------------------------------------------------------------------------
#ifdef NEED_WORLDPOS
    varying vec3 v_pos;
#endif

//----------------------------------------------------------------------------------------------------------------------
// directional light
#ifdef DIRECT_LIGHT_COUNT
    struct DirectLight {
        vec3 color;
        vec3 direction;
    };
    uniform vec3 u_directLightColor[DIRECT_LIGHT_COUNT];
    uniform vec3 u_directLightDirection[DIRECT_LIGHT_COUNT];
#endif


// point light
#ifdef POINT_LIGHT_COUNT
    struct PointLight {
        vec3 color;
        vec3 position;
        float distance;
    };
    uniform vec3 u_pointLightColor[POINT_LIGHT_COUNT];
    uniform vec3 u_pointLightPosition[POINT_LIGHT_COUNT];
    uniform float u_pointLightDistance[POINT_LIGHT_COUNT];
#endif


// spot light
#ifdef SPOT_LIGHT_COUNT
    struct SpotLight {
        vec3 color;
        vec3 position;
        vec3 direction;
        float distance;
        float angleCos;
        float penumbraCos;
    };
    uniform vec3 u_spotLightColor[SPOT_LIGHT_COUNT];
    uniform vec3 u_spotLightPosition[SPOT_LIGHT_COUNT];
    uniform vec3 u_spotLightDirection[SPOT_LIGHT_COUNT];
    uniform float u_spotLightDistance[SPOT_LIGHT_COUNT];
    uniform float u_spotLightAngleCos[SPOT_LIGHT_COUNT];
    uniform float u_spotLightPenumbraCos[SPOT_LIGHT_COUNT];
#endif

// ambient light
struct EnvMapLight {
    vec3 diffuse;
    float mipMapLevel;
    float diffuseIntensity;
    float specularIntensity;
};
uniform EnvMapLight u_envMapLight;

#ifdef USE_SH
    uniform vec3 u_env_sh[9];
#endif

#ifdef USE_SPECULAR_ENV
    uniform samplerCube u_env_specularSampler;
#endif

//----------------------------------------------------------------------------------------------------------------------
uniform vec4 u_emissiveColor;
uniform vec4 u_diffuseColor;
uniform vec4 u_specularColor;
uniform float u_shininess;
uniform float u_normalIntensity;
uniform float u_alphaCutoff;

#ifdef EMISSIVE_TEXTURE
    uniform sampler2D u_emissiveTexture;
#endif

#ifdef DIFFUSE_TEXTURE
    uniform sampler2D u_diffuseTexture;
#endif

#ifdef SPECULAR_TEXTURE
    uniform sampler2D u_specularTexture;
#endif

#ifdef NORMAL_TEXTURE
    uniform sampler2D u_normalTexture;
#endif

//----------------------------------------------------------------------------------------------------------------------
vec3 getNormal() {
    #ifdef NORMAL_TEXTURE
        #ifndef HAS_TANGENT
            #ifdef HAS_DERIVATIVES
                vec3 pos_dx = dFdx(v_pos);
                vec3 pos_dy = dFdy(v_pos);
                vec3 tex_dx = dFdx(vec3(v_uv, 0.0));
                vec3 tex_dy = dFdy(vec3(v_uv, 0.0));
                vec3 t = (tex_dy.t * pos_dx - tex_dx.t * pos_dy) / (tex_dx.s * tex_dy.t - tex_dy.s * tex_dx.t);
                #ifdef HAS_NORMAL
                    vec3 ng = normalize(v_normal);
                #else
                    vec3 ng = normalize(cross(pos_dx, pos_dy));
                #endif
                t = normalize(t - ng * dot(ng, t));
                vec3 b = normalize(cross(ng, t));
                mat3 tbn = mat3(t, b, ng);
            #else
                #ifdef HAS_NORMAL
                    vec3 ng = normalize(v_normal);
                #else
                    vec3 ng = vec3(0.0, 0.0, 1.0);
                #endif
                mat3 tbn = mat3(vec3(0.0), vec3(0.0), ng);
            #endif
        #else
            mat3 tbn = v_TBN;
        #endif
        vec3 n = texture2D(u_normalTexture, v_uv).rgb;
        n = normalize(tbn * ((2.0 * n - 1.0) * vec3(u_normalIntensity, u_normalIntensity, 1.0)));
    #else
        #ifdef HAS_NORMAL
            vec3 n = normalize(v_normal);
        #elif defined(HAS_DERIVATIVES)
            vec3 pos_dx = dFdx(v_pos);
            vec3 pos_dy = dFdy(v_pos);
            vec3 n = normalize(cross(pos_dx, pos_dy));
        #else
            vec3 n= vec3(0.0, 0.0, 1.0);
        #endif
    #endif

    n *= float(gl_FrontFacing) * 2.0 - 1.0;
    return n;
}

//----------------------------------------------------------------------------------------------------------------------
//**********************************************************************************************************************
//----------------------------------------------------------------------------------------------------------------------
void main() {
    vec4 ambient = vec4(0.0);
    vec4 emission = u_emissiveColor;
    vec4 diffuse = u_diffuseColor;
    vec4 specular = u_specularColor;

    #ifdef EMISSIVE_TEXTURE
        vec4 emissiveTextureColor = texture2D(u_emissiveTexture, v_uv);
//        #ifndef OASIS_COLORSPACE_GAMMA
//            emissiveTextureColor = gammaToLinear(emissiveTextureColor);
//        #endif
        emission *= emissiveTextureColor;
    #endif

    #ifdef DIFFUSE_TEXTURE
        vec4 diffuseTextureColor = texture2D(u_diffuseTexture, v_uv);
//        #ifndef OASIS_COLORSPACE_GAMMA
//            diffuseTextureColor = gammaToLinear(diffuseTextureColor);
//        #endif
        diffuse *= diffuseTextureColor;
    #endif

    #ifdef HAS_VERTEXCOLOR
        diffuse *= v_color;
    #endif

    #ifdef SPECULAR_TEXTURE
        vec4 specularTextureColor = texture2D(u_specularTexture, v_uv);
//        #ifndef OASIS_COLORSPACE_GAMMA
//            specularTextureColor = gammaToLinear(specularTextureColor);
//        #endif
        specular *= specularTextureColor;
    #endif

    ambient = vec4(u_envMapLight.diffuse * u_envMapLight.diffuseIntensity, 1.0) * diffuse;

    //------------------------------------------------------------------------------------------------------------------
    #ifdef NEED_WORLDPOS
        vec3 V =  normalize(u_cameraPos - v_pos);
    #endif

    //------------------------------------------------------------------------------------------------------------------
    vec3 N = getNormal();
    vec3 lightDiffuse = vec3(0.0, 0.0, 0.0);
    vec3 lightSpecular = vec3(0.0, 0.0, 0.0);

    #ifdef DIRECT_LIGHT_COUNT
        DirectLight directionalLight;
        for (int i = 0; i < DIRECT_LIGHT_COUNT; i++) {
            directionalLight.color = u_directLightColor[i];
            directionalLight.direction = u_directLightDirection[i];

            float d = max(dot(N, -directionalLight.direction), 0.0);
            lightDiffuse += directionalLight.color * d;

            vec3 halfDir = normalize(V - directionalLight.direction);
            float s = pow(clamp(dot(N, halfDir), 0.0, 1.0), u_shininess);
            lightSpecular += directionalLight.color * s;
        }
    #endif

    #ifdef POINT_LIGHT_COUNT
        PointLight pointLight;
        for (int i = 0; i < POINT_LIGHT_COUNT; i++) {
            pointLight.color = u_pointLightColor[i];
            pointLight.position = u_pointLightPosition[i];
            pointLight.distance = u_pointLightDistance[i];

            vec3 direction = v_pos - pointLight.position;
            float dist = length(direction);
            direction /= dist;
            float decay = clamp(1.0 - pow(dist / pointLight.distance, 4.0), 0.0, 1.0);

            float d =  max(dot(N, -direction), 0.0) * decay;
            lightDiffuse += pointLight.color * d;

            vec3 halfDir = normalize(V - direction);
            float s = pow(clamp(dot(N, halfDir), 0.0, 1.0), u_shininess)  * decay;
            lightSpecular += pointLight.color * s;
        }
    #endif

    #ifdef SPOT_LIGHT_COUNT
        SpotLight spotLight;
        for (int i = 0; i < SPOT_LIGHT_COUNT; i++) {
            spotLight.color = u_spotLightColor[i];
            spotLight.position = u_spotLightPosition[i];
            spotLight.direction = u_spotLightDirection[i];
            spotLight.distance = u_spotLightDistance[i];
            spotLight.angleCos = u_spotLightAngleCos[i];
            spotLight.penumbraCos = u_spotLightPenumbraCos[i];

            vec3 direction = spotLight.position - v_pos;
            float lightDistance = length(direction);
            direction /= lightDistance;
            float angleCos = dot(direction, -spotLight.direction);
            float decay = clamp(1.0 - pow(lightDistance/spotLight.distance, 4.0), 0.0, 1.0);
            float spotEffect = smoothstep(spotLight.penumbraCos, spotLight.angleCos, angleCos);
            float decayTotal = decay * spotEffect;
            float d = max(dot(N, direction), 0.0)  * decayTotal;
            lightDiffuse += spotLight.color * d;

            vec3 halfDir = normalize(V + direction);
            float s = pow(clamp(dot(N, halfDir), 0.0, 1.0), u_shininess) * decayTotal;
            lightSpecular += spotLight.color * s;
        }
    #endif

    diffuse *= vec4(lightDiffuse, 1.0);
    specular *= vec4(lightSpecular, 1.0);

    #ifdef ALPHA_CUTOFF
        if (diffuse.a < u_alphaCutoff) {
            discard;
        }
    #endif

    //------------------------------------------------------------------------------------------------------------------
    gl_FragColor = emission + ambient + diffuse + specular;
    gl_FragColor.a = diffuse.a;

//    #ifndef OASIS_COLORSPACE_GAMMA
//        gl_FragColor = linearToGamma(gl_FragColor);
//    #endif
}