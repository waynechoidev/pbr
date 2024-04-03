#version 420 core

out vec4 FragColor;
in vec2 TexCoord;
in vec3 posWorld;
in vec3 normalWorld;
in vec3 tangentWorld;

layout(std140) uniform Fragment
{
                          //size     index
    vec3 camPos;          //12       0
    bool useDirectLight;   //4        12
    //
    vec3 lightPos;        //12       16
    bool useEnvLight;     //4        28
                          //total    32
};

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
    // Apply gamma correction to the sampled albedo texture to convert it from sRGB space to linear space
    vec3 albedo = pow(texture(albedoMap, TexCoord).rgb, vec3(2.2));
    float metallic  = texture(metallicMap, TexCoord).r;
    float roughness = texture(roughnessMap, TexCoord).r;
    float ao = texture(aoMap, TexCoord).r;

    vec3 N = normalize(normalWorld);
    // Adjust the tangent vector to ensure it is perpendicular to the surface
    // by removing the component parallel to the normal vector.
    vec3 T = normalize(tangentWorld - dot(tangentWorld, N) * N);
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T, B, N);
    N = normalize(TBN * (texture(normalMap, TexCoord).xyz * 2.0 - 1.0));
    
    vec3 V = normalize(camPos - posWorld);

    vec3 lightRadiances = vec3(10.0);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

        vec3 L = normalize(lightPos - posWorld);
    vec3 H = normalize(V + L);
    float distance = length(lightPos - posWorld);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = lightRadiances * attenuation;

    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    float G = GeometrySmith(N, V, L, roughness);      
    float NDF = DistributionGGX(N, H, roughness);   
        
    vec3 numerator = NDF * G * F;
    // + 0.0001 to prevent divide by zero
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;
    
    vec3 kS = F;

    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);

    vec3 directLight = (kD * albedo / PI + specular) * radiance * NdotL;        

    vec3 ambient = vec3(0.3) * albedo * ao;

    vec3 color = ambient + directLight;
    
    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}