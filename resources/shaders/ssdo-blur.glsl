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

uniform sampler2D u_Input;
uniform vec2 u_PixelSize;

uniform float u_FilerSize;

out vec4 color;

void main() 
{
	color = vec4(0.0f);

	int r = int(u_FilerSize / 2.0f);
	int l = -r + (int(u_FilerSize) % 2 == 0 ? 1 : 0);

	vec4 centerColor = texture2D(u_Input, v_TextureCoordinates);

	float totalWeight = 0.0f;

	for (int i = l; i <= r; ++i) 
	{
		for (int j = l; j <= r; ++j) 
		{
			vec2 position = v_TextureCoordinates + vec2(i, j) * u_PixelSize;

			vec4 currentColor = texture2D(u_Input, position);
			vec4 delta = centerColor - currentColor;

			float weight = (1 - (delta.x * delta.x + delta.y + delta.y + delta.z * delta.z)) > 0.93 ? 1.0f : 0.0f; 

			color += weight * currentColor;
			totalWeight += weight;
		}
	}

	color /= totalWeight;
	color.a = 1.0f;
}
