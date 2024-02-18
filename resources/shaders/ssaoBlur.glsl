// type vertex

#version 460 core

layout(location = 0) in vec2 position;

void main() 
{
	gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
}

// type fragment

#version 460 core

#define MAX_SAMPLES_COUNT 64

uniform sampler2D u_AmbientOcclusion;
uniform vec3 u_AmbientOcclusion_PixelSize;
uniform float u_AmbientOcclusion_ActiveTexturePercentage;

uniform vec2 u_ScreenSize;

out float ambientOcclusion;

void main() 
{
	vec2 pos = gl_FragCoord.xy / u_ScreenSize;

	ambientOcclusion = 0.0f;
	for (int i = -2; i < 2; ++i) 
	{
		for (int j = -2; j < 2; ++j) 
		{
			vec2 position = pos + vec2(i, j) * u_AmbientOcclusion_PixelSize.xy;
			ambientOcclusion += texture2D(u_AmbientOcclusion, position * u_AmbientOcclusion_ActiveTexturePercentage).r;
		}
	}
	ambientOcclusion /= 16;
}
