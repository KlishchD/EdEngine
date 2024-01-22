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
#define PI 3.1415926535897932384626433832795

in vec2 v_TextureCoordinates;

uniform sampler2D u_Diffuse;
uniform sampler2D u_Position;
uniform sampler2D u_Normal;

uniform float u_SampleCount;
uniform vec3 u_Samples[MAX_SAMPLES_COUNT];

uniform float u_Radius = 1.0f;

uniform float u_BounceStrength = 1.0f;

uniform float u_NoiseSize;
uniform sampler2D u_Noise;

uniform vec2 u_ScreenSize;

uniform mat4 u_ViewMatrix;
uniform mat4 u_NormalMatrix;
uniform mat4 u_ProjectionMatrix;

out vec4 color;

void main() 
{
	vec2 noiseScale = u_ScreenSize / vec2(u_NoiseSize);

	vec3 position = (u_ViewMatrix * vec4(texture2D(u_Position, v_TextureCoordinates).xyz, 1.0f)).xyz;
	vec3 normal = (u_NormalMatrix * vec4(texture2D(u_Normal, v_TextureCoordinates).xyz, 1.0f)).xyz;

	vec3 noise = texture2D(u_Noise, v_TextureCoordinates * noiseScale).xyz;

	vec3 tangent = normalize(noise - normal * dot(normal, noise));
	vec3 bitangent = cross(normal, tangent);

	mat3 TBN = mat3(tangent, bitangent, normal);

	float A = PI * u_Radius * u_Radius / u_SampleCount;

	for (int i = 0; i < u_SampleCount && i < MAX_SAMPLES_COUNT; ++i)
	{
		vec3 direction = u_Radius * u_Samples[i];
		vec3 samplePosition = position + TBN * direction;
		
		vec4 screen = u_ProjectionMatrix * vec4(samplePosition, 1.0f);
		screen.xy /= screen.w;
		screen.xy = 0.5f * screen.xy + 0.5f;

		vec3 samplerScreenPosition = (u_ViewMatrix * vec4(texture2D(u_Position, screen.xy).xyz, 1.0f)).xyz;

		float visible = samplePosition.z >= samplerScreenPosition.z ? 1.0f : 0.0f;
		float distanceCheck = abs(samplePosition.z - samplerScreenPosition.z) < u_Radius ? 1.0f : 0.0f;

		vec3 sampleColor = texture2D(u_Diffuse, screen.xy).rgb;
		vec3 sampleNormal = (u_NormalMatrix * vec4(texture2D(u_Normal, screen.xy).xyz, 1.0f)).xyz;

		vec3 recieverSender = position - samplePosition;
		vec3 transmissionDirection = normalize(recieverSender);

		float F = A * max(dot(-u_Samples[i], sampleNormal), 0.0f) * max(dot(u_Samples[i], normal), 0.0f); // why negative
		//F /= PI * u_Radius * u_Radius + 0.0000001f; // dot(recieverSender, recieverSender)

		color += vec4(u_BounceStrength * (1.0f - visible) * F * sampleColor, 0.0f);
	}

	color *= 2 * PI / u_SampleCount; // color * d_w

	color.a = 1.0f;
}
