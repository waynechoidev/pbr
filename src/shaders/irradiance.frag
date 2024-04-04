#version 420 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube envCubemap;

const float PI = 3.14159265359;

void main()
{       
    vec3 N = normalize(WorldPos);

    const vec3 up = vec3(0.0, 1.0, 0.0);
    const float invSampleCount = 1.0 / 512.0; // Can be adjusted based on the number of samples.

    vec3 irradiance = vec3(0.0);

    // Sampling from the cubemap using importance sampling
    for (int i = 0; i < 512; ++i) {
        // Sampling direction for importance sampling
        vec3 sampleDir = normalize(texture(envCubemap, N).rgb * 2.0 - 1.0);

        // Calculating the cosine of the angle between sampling direction and surface normal
        float cosTheta = dot(sampleDir, N);

        irradiance += texture(envCubemap, sampleDir).rgb * cosTheta * invSampleCount;

        // Randomly rotating N direction for the next sampling direction
        N = normalize(N * 0.9 + sampleDir * 0.1);
    }

    irradiance = PI * irradiance;
    
    FragColor = vec4(irradiance, 1.0);
}
