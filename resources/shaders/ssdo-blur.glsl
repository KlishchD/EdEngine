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

in vec2 v_TextureCoordinates;

uniform sampler2D u_Input;
uniform vec2 u_PixelSize;

out vec4 color;

void main() 
{
	int hsize = 10;

	for (int i = -hsize; i < hsize; ++i) 
	{
		for (int j = -hsize; j < hsize; ++j) 
		{
			vec2 position = v_TextureCoordinates + vec2(i, j) * u_PixelSize;
			color += texture2D(u_Input, position);
		}
	}

	color /= 4 * hsize * hsize;
/*

	color += 4 * texture2D(u_Input, v_TextureCoordinates);

	color += 2 * texture2D(u_Input, v_TextureCoordinates + vec2(0.0f, u_PixelSize.y));
	color += 2 * texture2D(u_Input, v_TextureCoordinates + vec2(0.0f, -u_PixelSize.y));
	color += 2 * texture2D(u_Input, v_TextureCoordinates + vec2(u_PixelSize.x, 0.0f));
	color += 2 * texture2D(u_Input, v_TextureCoordinates + vec2(-u_PixelSize.x, 0.0f));

	color += texture2D(u_Input, v_TextureCoordinates + vec2(-u_PixelSize.x, u_PixelSize.y));
	color += texture2D(u_Input, v_TextureCoordinates + vec2(-u_PixelSize.x, -u_PixelSize.y));
	color += texture2D(u_Input, v_TextureCoordinates + vec2(u_PixelSize.x, u_PixelSize.y));
	color += texture2D(u_Input, v_TextureCoordinates + vec2(u_PixelSize.x, -u_PixelSize.y));

	color /= 16.0f;
*/
}
