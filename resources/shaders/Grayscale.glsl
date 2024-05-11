// type vertex

#version 430 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 textureCoordinates;

out vec2 v_TextureCoordinates;

void main() 
{
	gl_Position = vec4(position, 0.0f, 1.0f);
	v_TextureCoordinates = textureCoordinates;
}

// type fragment

#version 430 core

uniform sampler2D u_Color;

in vec2 v_TextureCoordinates;

out vec4 grayscale;

void main()
{
	vec3 color = texture2D(u_Color, v_TextureCoordinates).rgb;

	vec3 multiplier = vec3(0.2126, 0.7152, 0.0722);
	float scale = dot(multiplier, color);

	grayscale = vec4(scale, scale, scale, 1.0f);
}