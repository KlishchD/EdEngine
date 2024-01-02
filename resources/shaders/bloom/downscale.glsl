// type compute

#version 460 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

uniform sampler2D u_Input;
writeonly uniform image2D u_Output;

uniform vec2 u_PixelSize;

void main() 
{
	vec2 location = 1.0f * (gl_WorkGroupID.xy + 0.5f) / gl_NumWorkGroups.xy;
	
	/* basic
	// a-b
	// | |
	// c-d

	vec3 a = texture2D(u_Input, location).rgb;
	vec3 b = texture2D(u_Input, vec2(location.x + u_PixelSize.x, location.y)).rgb;
	vec3 c = texture2D(u_Input, vec2(location.x, location.y + u_PixelSize.y)).rgb;
	vec3 d = texture2D(u_Input, vec2(location.x + u_PixelSize.x, location.y + u_PixelSize.y)).rgb;

	vec3 average = (a + b + c + d) / 4.0f;

	*/

	// a - b - c
    // - d - e -
    // f - g - h
    // - i - j -
    // k - l - m

	vec3 a = texture2D(u_Input, location + vec2(-2.0f * u_PixelSize.x, 2.0f * u_PixelSize.y)).rgb;
	vec3 b = texture2D(u_Input, location + vec2(0.0f,                  2.0f * u_PixelSize.y)).rgb;
	vec3 c = texture2D(u_Input, location + vec2(2.0f * u_PixelSize.x,  2.0f * u_PixelSize.y)).rgb;
	
	vec3 d = texture2D(u_Input, location + vec2(-u_PixelSize.x, u_PixelSize.y)).rgb;
	vec3 e = texture2D(u_Input, location + vec2( u_PixelSize.x, u_PixelSize.y)).rgb;
	
	vec3 f = texture2D(u_Input, location + vec2(-2.0f * u_PixelSize.x, 0.0f)).rgb;
	vec3 g = texture2D(u_Input, location).rgb;
	vec3 h = texture2D(u_Input, location + vec2(2.0f * u_PixelSize.x, 0.0f)).rgb;
	
	vec3 i = texture2D(u_Input, location + vec2(-u_PixelSize.x, -u_PixelSize.y)).rgb;
	vec3 j = texture2D(u_Input, location + vec2( u_PixelSize.x, -u_PixelSize.y)).rgb;

	vec3 k = texture2D(u_Input, location + vec2(-2.0f * u_PixelSize.x, -2.0f * u_PixelSize.y)).rgb;
	vec3 l = texture2D(u_Input, location + vec2(0.0f,                  -2.0f * u_PixelSize.y)).rgb;
	vec3 m = texture2D(u_Input, location + vec2(2.0f * u_PixelSize.x,  -2.0f * u_PixelSize.y)).rgb;
	
	vec3 average = (0.125f * (a + c + k + m) + 0.250f * (b + f + h + l) + 0.5f * (d + e + i + j + g)) / 3.5f;

	imageStore(u_Output, ivec2(gl_WorkGroupID.x, gl_WorkGroupID.y), vec4(average, 1.0f));
}