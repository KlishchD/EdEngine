// type fragment

#version 430 core

uniform sampler2D u_IconTexture;

in vec2 v_TextureCoordinates;

layout(location = 0) out vec4 color;
layout(location = 3) out vec4 roughnessMetalic;

void main() {
    color = texture(u_IconTexture, v_TextureCoordinates);
    roughnessMetalic.a = 0;
}

// type vertex

#version 430 core

uniform mat4 u_ProjectionViewMatrix;
uniform mat4 u_ModelTransform;

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 textureCoordinates;

out vec2 v_TextureCoordinates;

void main() {
    v_TextureCoordinates = textureCoordinates;
    gl_Position = u_ProjectionViewMatrix * u_ModelTransform * position;
}
