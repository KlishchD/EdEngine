// type compute

#version 460 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

uniform bool u_UseFullsize;
uniform sampler2D u_Fullsize;

uniform sampler2D u_Downscaled;
layout(r11f_g11f_b10f) uniform image2D u_Upscaled;

uniform float u_MixStrength;

uniform vec2 u_PixelSize;

void main() 
{
	// 1 2 1
	// 2 4 2
	// 1 2 1

	// a b c
	// d e f
	// g h i

	vec2 location = 1.0f * (gl_WorkGroupID.xy + 0.5f) / gl_NumWorkGroups.xy;
	ivec2 imageLocation = ivec2(gl_WorkGroupID.x, gl_WorkGroupID.y);

	vec3 color;
	if (u_UseFullsize)
	{
		color = texture2D(u_Fullsize, location).rgb;
	}
	else
	{
		color = imageLoad(u_Upscaled, imageLocation).rgb;
	}

	vec3 a = texture2D(u_Downscaled, location + vec2(-u_PixelSize.x, u_PixelSize.y)).rgb;
	vec3 b = texture2D(u_Downscaled, location + vec2(0.0f,           u_PixelSize.y)).rgb;
	vec3 c = texture2D(u_Downscaled, location + vec2(u_PixelSize.x,  u_PixelSize.y)).rgb;

	vec3 d = texture2D(u_Downscaled, location + vec2(-u_PixelSize.x, 0.0f)).rgb;
	vec3 e = texture2D(u_Downscaled, location).rgb;
	vec3 f = texture2D(u_Downscaled, location + vec2(u_PixelSize.x, 0.0f)).rgb;

	vec3 g = texture2D(u_Downscaled, location + vec2(-u_PixelSize.x, -u_PixelSize.y)).rgb;
	vec3 h = texture2D(u_Downscaled, location + vec2(0.0f,           -u_PixelSize.y)).rgb;
	vec3 i = texture2D(u_Downscaled, location + vec2(u_PixelSize.x,  -u_PixelSize.y)).rgb;

	
	vec3 average = (a + c + g + i) + 2 * (b + d + f + h) + 4 * e;
	average /= 16.0f;

	imageStore(u_Upscaled, imageLocation, vec4(mix(vec3(0.0f), color + average, u_MixStrength), 1.0f));
}