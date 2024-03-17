// type vertex

#version 460 core

layout(location = 0) in vec2 position;

void main()
{
	gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
}

// type fragment

#version 460 core

uniform bool u_UseFullsize;
uniform sampler2D u_Fullsize;

uniform sampler2D u_Downscaled;
uniform sampler2D u_Upscaled;

uniform float u_MixStrength;

uniform vec2 u_UpscaledPixelSize;
uniform vec2 u_DownscaledPixelSize;

out vec4 color;

void main() 
{
	// 1 2 1
	// 2 4 2
	// 1 2 1

	// a b c
	// d e f
	// g h i

	vec2 location = gl_FragCoord.xy * u_UpscaledPixelSize;

	vec3 upsacledColor = texture2D(u_Upscaled, location).rgb;
	
	vec3 a = texture2D(u_Downscaled, location + vec2(-u_DownscaledPixelSize.x, u_DownscaledPixelSize.y)).rgb;
	vec3 b = texture2D(u_Downscaled, location + vec2(0.0f,           u_DownscaledPixelSize.y)).rgb;
	vec3 c = texture2D(u_Downscaled, location + vec2(u_DownscaledPixelSize.x,  u_DownscaledPixelSize.y)).rgb;

	vec3 d = texture2D(u_Downscaled, location + vec2(-u_DownscaledPixelSize.x, 0.0f)).rgb;
	vec3 e = texture2D(u_Downscaled, location).rgb;
	vec3 f = texture2D(u_Downscaled, location + vec2(u_DownscaledPixelSize.x, 0.0f)).rgb;

	vec3 g = texture2D(u_Downscaled, location + vec2(-u_DownscaledPixelSize.x, -u_DownscaledPixelSize.y)).rgb;
	vec3 h = texture2D(u_Downscaled, location + vec2(0.0f,           -u_DownscaledPixelSize.y)).rgb;
	vec3 i = texture2D(u_Downscaled, location + vec2(u_DownscaledPixelSize.x,  -u_DownscaledPixelSize.y)).rgb;

	
	vec3 average = (a + c + g + i) + 2 * (b + d + f + h) + 4 * e;
	average /= 16.0f;

	color = vec4(mix(vec3(0.0f), upsacledColor + average, u_MixStrength), 1.0f);
}