// type vertex

#version 460 core

layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 TextureCoordinates;

out vec2 v_TextureCoordinates;

void main() {       
    gl_Position = vec4(Position.x, Position.y, 0.0f, 1.0f);
    v_TextureCoordinates = TextureCoordinates;
}

// type fragment

#version 460 core

uniform sampler2D u_Image;
uniform bool u_HorizontalPass;

uniform float u_PixelWidth;
uniform float u_PixelHeight;

uniform vec2 u_ScreenSize;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

in vec2 v_TextureCoordinates;

layout(location = 0) out vec4 Color;

void main() {
    vec3 result = texture(u_Image, v_TextureCoordinates).xyz * weight[0];

    if (u_HorizontalPass) 
    {
        for (int i = 1; i < 5; ++i) {
            result += texture(u_Image, v_TextureCoordinates - vec2(i * u_PixelWidth, 0.0f)).xyz * weight[i];
            result += texture(u_Image, v_TextureCoordinates + vec2(i * u_PixelWidth, 0.0f)).xyz * weight[i];
        }
    }
    else 
    {
        for (int i = 1; i < 5; ++i) {
            result += texture(u_Image, v_TextureCoordinates - vec2(0.0f, i * u_PixelWidth)).xyz * weight[i];
            result += texture(u_Image, v_TextureCoordinates + vec2(0.0f, i * u_PixelWidth)).xyz * weight[i];
        }
    }

    Color = vec4(result, 1.0f);
}
