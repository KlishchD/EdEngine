// type vertex

#version 460 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 textureCoordinates;

out vec2 v_TextureCoordinates;

void main() 
{
	gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
	v_TextureCoordinates = textureCoordinates;
}

// type fragment

#version 460 core

#define MAX_SAMPLES_COUNT 64

in vec2 v_TextureCoordinates;

uniform sampler2D u_Position;
uniform sampler2D u_Normal;

uniform float u_SampleCount;
uniform vec3 u_Samples[MAX_SAMPLES_COUNT];

uniform float u_NoiseSize;
uniform sampler2D u_Noise;

uniform vec2 u_ScreenSize;

uniform mat4 u_ViewMatrix;
uniform mat4 u_NormalMatrix;
uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ProjectionViewMatrix;

uniform float u_Radius;
uniform float u_Bias;

out float color;

void main() 
{
	vec2 noiseScale = u_ScreenSize / vec2(u_NoiseSize);

	vec3 position = (u_ViewMatrix * vec4(texture2D(u_Position, v_TextureCoordinates).xyz, 1.0f)).xyz;
	vec3 normal = (u_NormalMatrix * vec4(texture2D(u_Normal, v_TextureCoordinates).xyz, 1.0f)).xyz;

	vec3 noise = texture2D(u_Noise, v_TextureCoordinates * noiseScale).xyz;
	vec3 tanget = normalize(noise - normal * dot(noise, normal)); // TODO: How does this work

	vec3 bitanget = cross(normal, tanget);

	mat3 TBN = mat3(tanget, bitanget, normal); // tanget-scape to view

	float occluders = 0.0f;
	for (int i = 0; i < u_SampleCount && i < MAX_SAMPLES_COUNT; ++i) 
	{
		vec3 samplePosition = position + TBN * u_Samples[i] * u_Radius;

		vec4 screen = u_ProjectionMatrix * vec4(samplePosition, 1.0f);
		screen.xyz /= screen.w;
		screen = screen * 0.5f + 0.5f;

		float screenDepth = (u_ViewMatrix * texture2D(u_Position, screen.xy)).z;
		float sampleDepth = samplePosition.z;

		float rangeCheck = smoothstep(0.0f, 1.0f, u_Radius / abs(position.z - screenDepth));

		occluders += (screenDepth - u_Bias >= sampleDepth ? 1.0f : 0.0f) * rangeCheck;
	}
	color = 1.0f - occluders / u_SampleCount;
}
