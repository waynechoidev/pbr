#version 420 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec2 tex;

out vec3 posWorld;
out vec3 normalWorld;
out vec3 tangentWorld;
out vec2 TexCoord;

layout(std140) uniform Vertices
{
	mat4 model; // 64			0
	mat4 view; // 64		64
	mat4 projection; // 64			128
	float heightScale; // 4			192
	// 196
};

layout(binding = 2) uniform sampler2D heightTex;

void main()
{
	// ignore scale
	normalWorld = normalize(mat3(transpose(inverse(model))) * norm);
	tangentWorld = normalize(model * vec4(tangent, 1.0)).xyz;
	TexCoord = tex;

	float height = texture(heightTex, tex).r;
	height = height * 2.0 - 1.0; // 0~1 -> -1~1
	vec3 newPos = pos + vec3(normalWorld * height * heightScale);

	gl_Position = projection * view * model * vec4(newPos, 1.0);

	posWorld = (model * vec4(newPos, 1.0)).xyz;
}