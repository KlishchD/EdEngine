﻿// type vertex

#version 460 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 textureCoordinates;

out vec2 v_TextureCoordinates;

void main() 
{
    gl_Position = vec4(position, 0.0f, 1.0f);
	v_TextureCoordinates = textureCoordinates;
}

// type fragment

#version 460 core

in vec2 v_TextureCoordinates;

uniform sampler2D u_Light;
uniform float u_Light_ActiveTexturePercentage;

uniform sampler2D u_Bloom;
uniform float u_Bloom_ActiveTexturePercentage;

uniform float u_BloomStrength;
uniform float u_BloomIntensity;

uniform float u_Gamma;

layout(location = 0) out vec4 result;

vec3 ACESFilm(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main()
{
    vec3 color = texture(u_Light, v_TextureCoordinates * u_Light_ActiveTexturePercentage).xyz;
    vec3 bloom = texture(u_Bloom, v_TextureCoordinates * u_Bloom_ActiveTexturePercentage).xyz * u_BloomIntensity;

    color = mix(color, bloom, u_BloomStrength);

    color = ACESFilm(color);
    
    color = pow(color, vec3(1.0f / u_Gamma));
    
    result = vec4(color, 1.0f);
}
