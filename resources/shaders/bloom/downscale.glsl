// type vertex

#version 460 core

layout(location = 0) in vec2 position;

void main()
{
	gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
}

// type fragment

#version 460 core

uniform sampler2D u_Input;
uniform vec2 u_InPixelSize;
uniform vec2 u_OutPixelSize;

out vec4 color;

void main() 
{
	vec2 location = gl_FragCoord.xy * u_OutPixelSize;
	
	/* basic
	// a-b
	// | |
	// c-d

	vec3 a = texture2D(u_Input, location).rgb;
	vec3 b = texture2D(u_Input, vec2(location.x + u_InPixelSize.x, location.y)).rgb;
	vec3 c = texture2D(u_Input, vec2(location.x, location.y + u_InPixelSize.y)).rgb;
	vec3 d = texture2D(u_Input, vec2(location.x + u_InPixelSize.x, location.y + u_InPixelSize.y)).rgb;

	vec3 average = (a + b + c + d) / 4.0f;

	*/

	// a - b - c
    // - d - e -
    // f - g - h
    // - i - j -
    // k - l - m

	vec3 a = texture2D(u_Input, location + vec2(-2.0f * u_InPixelSize.x, 2.0f * u_InPixelSize.y)).rgb;
	vec3 b = texture2D(u_Input, location + vec2(0.0f,                  2.0f * u_InPixelSize.y)).rgb;
	vec3 c = texture2D(u_Input, location + vec2(2.0f * u_InPixelSize.x,  2.0f * u_InPixelSize.y)).rgb;
	
	vec3 d = texture2D(u_Input, location + vec2(-u_InPixelSize.x, u_InPixelSize.y)).rgb;
	vec3 e = texture2D(u_Input, location + vec2( u_InPixelSize.x, u_InPixelSize.y)).rgb;
	
	vec3 f = texture2D(u_Input, location + vec2(-2.0f * u_InPixelSize.x, 0.0f)).rgb;
	vec3 g = texture2D(u_Input, location).rgb;
	vec3 h = texture2D(u_Input, location + vec2(2.0f * u_InPixelSize.x, 0.0f)).rgb;
	
	vec3 i = texture2D(u_Input, location + vec2(-u_InPixelSize.x, -u_InPixelSize.y)).rgb;
	vec3 j = texture2D(u_Input, location + vec2( u_InPixelSize.x, -u_InPixelSize.y)).rgb;

	vec3 k = texture2D(u_Input, location + vec2(-2.0f * u_InPixelSize.x, -2.0f * u_InPixelSize.y)).rgb;
	vec3 l = texture2D(u_Input, location + vec2(0.0f,                  -2.0f * u_InPixelSize.y)).rgb;
	vec3 m = texture2D(u_Input, location + vec2(2.0f * u_InPixelSize.x,  -2.0f * u_InPixelSize.y)).rgb;
	
	vec3 average = (0.125f * (a + c + k + m) + 0.250f * (b + f + h + l) + 0.5f * (d + e + i + j + g)) / 3.5f;

	color = vec4(average, 1.0f);
}