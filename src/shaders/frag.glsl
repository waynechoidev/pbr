#version 420 core

in vec2 TexCoord;
in vec3 posWorld;
in vec3 normalWorld;
in vec3 tangentWorld;

out vec4 colour;

layout(std140) uniform Fragment
{
    vec3 viewPosition; // 12    0
    bool useDiffuse; // 4       12
    bool useNormal; // 4        16
    // 20
};

layout(std140) uniform Material
{
    float ambient; // 4     0
    float shininess; // 4   4
    float diffuse; // 4     8
    float specular; // 4    12
    // 16
} material;

layout(std140) uniform Light
{
    vec3 position; // 12        0
    float strength; // 4        12
    vec3 direction; // 12       16
    float fallOffStart; // 4    28
    float fallOffEnd; // 4      32
    float spotPower; // 4       36
    // 40
} light;

layout(binding = 1) uniform sampler2D diffuseTex;
layout(binding = 2) uniform sampler2D normalTex;
layout(binding = 3) uniform sampler2D metallicTex;
layout(binding = 4) uniform sampler2D roughnessTex;
layout(binding = 5) uniform sampler2D aoTex;

float calcAttenuation(float d, float falloffStart, float falloffEnd)
{
    // Linear falloff
    return clamp((falloffEnd - d) / (falloffEnd - falloffStart), 0.0, 1.0);
}

vec3 blinnPhong(vec3 lightStrength, vec3 lightVec, vec3 normal, vec3 toEye)
{
    vec3 halfway = normalize(toEye + lightVec);
    float hdotn = dot(halfway, normal);
    vec3 specular = vec3(material.specular) * pow(max(hdotn, 0.0f), material.shininess);
    return material.ambient + (vec3(material.diffuse) + specular) * lightStrength;
}

vec3 computePointLight(vec3 pos, vec3 normal, vec3 toEye)
{
    vec3 lightVec = light.position - pos;

    float d = length(lightVec);

    if (d > light.fallOffEnd)
    {
        return vec3(0.0f);
    }
    else
    {
        lightVec /= d;
        
        float ndotl = max(dot(lightVec, normal), 0.0f);
        vec3 lightStrength = vec3(light.strength) * ndotl;
        
        float att = calcAttenuation(d, light.fallOffStart, light.fallOffEnd);
        lightStrength *= att;
        
        return blinnPhong(lightStrength, lightVec, normal, toEye);
    }
}

void main()
{
    vec3 N = normalize(normalWorld);
    vec3 T = normalize(tangentWorld - dot(tangentWorld, N) * N);
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T, B, N);
    vec3 normalMap = texture(normalTex, TexCoord).xyz;
    vec3 normal = normalize(TBN * normalMap);

	vec3 toEye = normalize(viewPosition - posWorld);

	vec3 res = vec3(0.0);
    
    res += computePointLight(posWorld, useNormal ? normal : normalWorld, toEye);

	colour = useDiffuse ? vec4(res, 1.0) * texture(diffuseTex, TexCoord) : vec4(res, 1.0);
}