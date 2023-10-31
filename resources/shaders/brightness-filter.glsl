// type vertex

#version 460 core

layout(location = 0) in vec2 Position;

void main() {       
    gl_Position = vec4(Position.x, Position.y, 0.0f, 1.0f);
}

// type fragment

#version 460 core

uniform sampler2D u_Image;

uniform vec2 u_ScreenSize;

layout(location = 0) out vec4 Color;

void main() {
    vec2 pos = gl_FragCoord.xy / u_ScreenSize;
    vec3 color = texture(u_Image, pos).xyz; 
    float brighness = color.r + color.g + color.b;

    if (brighness > 0.0f) 
    {
        Color = vec4(color, 1.0f);
    }
    else 
    {
        Color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
}
