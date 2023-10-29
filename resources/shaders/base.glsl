// type fragment

#version 430 core

uniform float u_Time;
uniform vec3 u_Resolution;
uniform sampler2D u_Texture;

in vec2 v_TextureCoordinates;

out vec4 color;

void main() {
    color = texture(u_Texture, v_textureCoordinates);

    vec3 timeColor = vec3(abs(sin(u_Time)), abs(cos(u_Time)), abs(sin(u_Time) + cos(u_Time)) / 2);
    color += vec4(timeColor * gl_FragCoord.xyz / u_Resolution, 1) / vec4(2, 2, 2, 1);

    /*
    if ((v_TexCoords.x < 0.01 || v_TexCoords.x > 0.99) || (v_TexCoords.y < 0.01 || v_TexCoords.y > 0.99)) {
        color += vec4(0.01, 0.01, 0.01, 1);
    }
    else {
        color += vec4(timeColor * gl_FragCoord.xyz / u_Resolution, 1) / vec4(10, 10, 1000, 1);
    }
    */
}

// type vertex

#version 430 core

uniform mat4 u_CameraMatrix;
uniform mat4 u_Transformation;

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec4 color;
layout(location = 2) in vec3 textureCoordinates;
layout(location = 3) in vec3 normal;

out vec2 v_TextureCoordinates;

void main() {
    gl_Position = u_CameraMatrix * u_Transformation * vec4(vertex, 1);
    v_TextureCoordinates = textureCoordinates;
}