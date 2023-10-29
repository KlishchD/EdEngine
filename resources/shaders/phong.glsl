// type fragment

#version 430 core

// TODO: UPDATE 

struct Light {
    vec3 Position;
    vec4 AmbientColor;
    vec4 DiffuseColor;
    vec4 SpecularColor;
};

struct Material {
    vec4 AmbientColor;
    vec4 DiffuseColor;
    vec4 SpecularColor;
    float Shininess;
    
    sampler2D BaseColorTexture;
    sampler2D NormalTexture;
};

uniform Light u_Light;
uniform Material u_Material;
uniform vec3 u_ViewPosition;

in vec3 v_TextureCoordinates;
in vec3 v_Normal;
in vec3 v_Position;
in vec4 v_BaseColor;

out vec4 color;

void main() {
    vec4 BaseColor = v_BaseColor * texture(u_Material.BaseColorTexture, v_TextureCoordinates.xy);
    vec3 Normal = v_Normal * texture(u_Material.NormalTexture, v_TextureCoordinates.xy).xyz;

    vec4 ambientColor = 0.1f * u_Material.AmbientColor * u_Light.AmbientColor;

    vec3 lightDirection = normalize(u_Light.Position - v_Position);
    
    float diffuse = max(dot(Normal, lightDirection), 0.0f);
    vec4 diffuseColor = diffuse * u_Material.DiffuseColor * u_Light.DiffuseColor;

    vec3 viewDirection = normalize(u_ViewPosition - v_Position);
    vec3 reflectionDirection = reflect(-lightDirection, Normal);
    
    float specular = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 32);
    vec4 specularColor = specular * u_Material.SpecularColor * u_Light.SpecularColor;

    color = (ambientColor + diffuseColor + specularColor) * BaseColor;
    color.a = 1.0f;
}

// type vertex

#version 430 core

uniform mat4 u_ProjectionViewMatrix;
uniform mat4 u_ModelMatrix;

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec4 BaseColor;
layout(location = 2) in vec3 textureCoordinates;
layout(location = 3) in vec3 normal;

out vec3 v_Position;
out vec4 v_BaseColor;
out vec3 v_TextureCoordinates;
out vec3 v_Normal;

void main() {       
    v_Position = (u_ModelMatrix * vec4(vertex, 1.0f)).xyz;
    v_BaseColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    v_TextureCoordinates = textureCoordinates;
    v_Normal = vec3(1.0f, 1.0f, 1.0f); // mat3(transpose(inverse(u_ModelMatrix))) * normal;
    
    gl_Position = u_ProjectionViewMatrix * u_ModelMatrix * vec4(vertex, 1.0f);
}