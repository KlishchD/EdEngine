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

uniform sampler2D u_AmbientOcclusion;
uniform vec2 u_PixelSize;

out float ambientOcclusion;

void main() 
{
	ambientOcclusion = 0.0f;
	for (int i = -2; i < 2; ++i) 
	{
		for (int j = -2; j < 2; ++j) 
		{
			vec2 position = v_TextureCoordinates + vec2(i, j) * u_PixelSize;
			ambientOcclusion += texture2D(u_AmbientOcclusion, position).r;
		}
	}
	ambientOcclusion /= 16;
}
