// type fragment

#version 430 core

#define M_PI 3.1415926535897932384626433832795
#define MAX_LIGHT_COUNT 10

struct Light {
    vec3 Position;
    vec3 Color;
    float Intensity;
};

struct Material {
    vec4 BaseColor;
    
    float Roughness;
    float Metalic;
    
    sampler2D BaseColorTexture;
    sampler2D NormalTexture;

    sampler2D RoughnessTexture;
    sampler2D MetalicTexture;
};

uniform Material u_Material;

uniform Light u_Light[MAX_LIGHT_COUNT];
uniform vec3 u_ViewPosition;
uniform float u_LightCount;

in vec3 v_Position;
in vec3 v_Normal;
in vec4 v_BaseColor;
in vec3 v_TextureCoordinates;
in mat3 v_TBN;

layout(location = 0) out vec4 color;

/*
float GX(float NdotH, float XdotH, float XdotN, float r2, float t2) {
    if (XdotH / XdotN > 0.0f) 
    {
        return 2.0f / (1.0f + sqrt(1.0f + r2 * t2));
    }
    return 0.0f;    
}
*/

float GX(float XdotV, float r) {
    float k = (1.0f + r) * (1.0f + r) / 8.0f;
    return XdotV / (XdotV * (1 - k) + k);
}

vec3 ACESFilm(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0,1.0);
}

void main() {
    vec3 albedo = (v_BaseColor * u_Material.BaseColor * texture(u_Material.BaseColorTexture, v_TextureCoordinates.xy)).xyz;
    float roughness = u_Material.Roughness * texture(u_Material.RoughnessTexture, v_TextureCoordinates.xy).r;
    float metalic = u_Material.Metalic * texture(u_Material.MetalicTexture, v_TextureCoordinates.xy).r;
    
    vec3 n;
    if (int(u_Material.NormalTexture) == 0) {
        n = normalize(v_Normal);
    } else {
        n = normalize(v_TBN * (2.0f * texture(u_Material.NormalTexture, v_TextureCoordinates.xy).xyz - 1.0f));
    }
    
    vec3 v = normalize(u_ViewPosition - v_Position);
    
    float NdotV = max(dot(n, v), 0.0f);
    
    float r2 = roughness * roughness;

    vec3 totalColor = vec3(0.0f);
    
    for (int i = 0; i < u_LightCount; ++i) {
        vec3 l = normalize(u_Light[i].Position - v_Position);
        vec3 h = normalize(l + v);
        
        float HdotV = max(dot(h, v), 0.0f);
        float NdotH = max(dot(n, h), 0.0f);
        float NdotL = max(dot(n, l), 0.0f);     
        float LdotH = max(dot(l, h), 0.0f);
        
        float NdotH2 = NdotH * NdotH;
    
        vec3 F0 = mix(vec3(0.04f), albedo, metalic);
        vec3 F = F0 + (vec3(1.0f) - F0) * pow(clamp((1.0f - HdotV), 0.0f, 1.0f), 5.0f);
    
        float denominator = (NdotH2 * (r2 * r2 - 1.0f) + 1.0f);
        float D = r2 * r2 / (M_PI * denominator * denominator); // r2 * r2 is Epic stuff ;)
        
        //float t2 = (1 - NdotH2) / NdotH2;
        //float G = GX(NdotH, LdotH, NdotL, r2, t2) * GX(NdotH, HdotV, NdotV, r2, t2);
        
        float G = GX(NdotV, roughness) * GX(NdotL, roughness);
        
        vec3 specular = F * G * D / (4.0f * NdotV * NdotL + 0.00001f); 
        vec3 diffuse = (vec3(1.0f) - F) * albedo / M_PI;
        
        float distance = length(u_Light[i].Position - v_Position);
        vec3 radiance = u_Light[i].Intensity * u_Light[i].Color / (distance);
        
        vec3 ambient = 0.03f * albedo; // Why 0.03?;)
        vec3 lColor = (diffuse + specular) * radiance * NdotL;
        vec3 res = lColor + ambient;
        
        totalColor += res;
    }
    
    // HDR tone-mapping
    totalColor = totalColor / (totalColor + vec3(1.0f));
    //totalColor = ACESFilm(totalColor);
    
    // gamma correction
    totalColor = pow(totalColor, vec3(1.0f / 2.2f));
    
    color = vec4(totalColor, 1.0f);
}

// type geometry

#version 430 core

in DATA {
    vec3 Position;
    vec3 Normal;
    vec4 BaseColor;
    vec3 TextureCoordinates;
    mat3 TBN;
} data_in[];

out vec3 v_Position;
out vec3 v_Normal;
out vec4 v_BaseColor;
out vec3 v_TextureCoordinates;
out mat3 v_TBN;

layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 3) out;

void main() {
    for (int i = 0; i < 3; ++i) {
        v_Position = data_in[i].Position;
        v_Normal = data_in[i].Normal;
        v_BaseColor = data_in[i].BaseColor;
        v_TextureCoordinates = data_in[i].TextureCoordinates;
        v_TBN = data_in[i].TBN;
        gl_Position = gl_in[i].gl_Position;
        
        EmitVertex();
    }
    
    EndPrimitive();
}

// type vertex

#version 430 core

uniform mat4 u_ProjectionViewMatrix;
uniform mat4 u_ModelMatrix;

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec4 BaseColor;
layout(location = 2) in vec3 textureCoordinates;
layout(location = 3) in vec3 normal;
layout(location = 4) in vec3 tangent;

out DATA {
    vec3 Position;
    vec3 Normal;
    vec4 BaseColor;
    vec3 TextureCoordinates;
    mat3 TBN;
} data_out;

void main() {   
    data_out.Position = (u_ModelMatrix * vec4(vertex, 1.0f)).xyz;
    data_out.Normal = transpose(inverse(u_ModelMatrix)) * normal;
    data_out.BaseColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    data_out.TextureCoordinates = textureCoordinates;
    
    vec3 T = normalize(vec3(inverse(transpose(u_ModelMatrix)) * vec4(tangent, 0.0f)));
    vec3 N = normalize(vec3(inverse(transpose(u_ModelMatrix)) * vec4(normal, 0.0f)));
    vec3 B = normalize(cross(normal, tangent));
    
    data_out.TBN = mat3(T, B, N);
    
    gl_Position = u_ProjectionViewMatrix * u_ModelMatrix * vec4(vertex, 1.0f);
}