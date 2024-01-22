// type vertex

#version 460 core

uniform mat4 u_ProjectionViewMatrix;

layout(location = 0) in vec2 position;

void main() {
    gl_Position = u_ProjectionViewMatrix * vec4(position, 0.0f, 1.0f);
}

// type fragment

#version 460 core

uniform sampler2D u_Albedo;

uniform float u_SampleCount;
uniform vec3 u_Samples[MAX_SAMPLES_COUNT];

uniform float u_Radius = 0.5f;

uniform float u_NoiseSize;
uniform sampler2D u_Noise;

uniform mat4 u_ViewMatrix;
uniform mat4 u_NormalMatrix;
uniform mat4 u_ProjectionMatrix;

uniform PointLight u_PointLight;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

layout(location = 0) out vec4 color;

float GX(float dot, float r) {
    float k = (1.0f + r) * (1.0f + r) / 8.0f;
    return dot / (dot * (1 - k) + k);
}

struct Lighting
{
    vec3 diffuse;
    vec3 specular;
};

Lighting CookTaurance(vec2 pos)
{
    Lighting lighting;
    
    vec3 albedo = texture(u_Albedo, pos).xyz;
    vec3 position = texture(u_Position, pos).xyz;
    vec3 normal = texture(u_Normal, pos).xyz;
    
    vec4 roughnessMetalic = texture(u_RoughnessMetalic, pos);
    float roughness = roughnessMetalic.x;
    float metalic = roughnessMetalic.y;

    if (roughnessMetalic.a == 1.0f) 
    {
        vec3 view = normalize(u_ViewPosition - position);

        float NdotV = max(dot(normal, view), 0.0f);

        float r2 = roughness * roughness;

        vec3 F0 = mix(vec3(0.04f), albedo, metalic);

        vec3 light = normalize(u_PointLight.Position - position);
        vec3 h = normalize(light + view);

        float HdotV = max(dot(h, view), 0.0f);
        float NdotH = max(dot(normal, h), 0.0f);
        float NdotL = max(dot(normal, light), 0.0f);
        float LdotH = max(dot(light, h), 0.0f);

        float NdotH2 = NdotH * NdotH;

        vec3 F = F0 + (vec3(1.0f) - F0) * pow(clamp((1.0f - HdotV), 0.0f, 1.0f), 5.0f);

        float denominator = (NdotH2 * (r2 * r2 - 1.0f) + 1.0f);
        float D = r2 * r2 / (M_PI * denominator * denominator + 0.0001f); // r2 * r2 is Epic stuff ;)

        //float t2 = (1 - NdotH2) / NdotH2;
        //float G = GX(NdotH, LdotH, NdotL, r2, t2) * GX(NdotH, HdotV, NdotV, r2, t2);

        float G = GX(NdotV, roughness) * GX(NdotL, roughness);

        vec3 specular = F * G * D / (4.0f * NdotV * NdotL + 0.0001f);
        vec3 diffuse = (vec3(1.0f) - F) * albedo / M_PI;

        float distance = length(u_PointLight.Position - position);
        vec3 radiance = u_PointLight.Intensity * u_PointLight.Color / (distance * distance);

        if (distance > u_PointLight.Radius) {
            discard;
        }

        float bias = max(5.0f * (1.0f - NdotL), 1.0f);
        float shadowIntensity = 0;

        /*

        float diskRadius = 0.005f;// (1.0f + (length(u_ViewPosition - position) / u_FarPlane)) / 25.0f;

        for (int i = 0; i < 20; ++i) 
        {
            float nearest = texture(u_PointLight.ShadowMap, -light.xyz + sampleOffsetDirections[i] * diskRadius).r * u_FarPlane;
            if (nearest + bias < distance)  
            {
                shadowIntensity += 1;
            }
        }

        shadowIntensity /= 20;
        */
        if (u_PointLight.UseShadowMap)
        {
            float offset = 0.01f;
            float delta = (2.0f * offset) / u_FilterSize;

            for (float i = -offset; i < offset; i += delta)
            {
                for (float j = -offset; j < offset; j += delta)
                {
                    for (float k = -offset; k < offset; k += delta)
                    {
                        float nearest = texture(u_PointLight.ShadowMap, -light.xyz + vec3(i, j, k)).r * u_FarPlane;
                        if (nearest + bias < distance)
                        {
                            shadowIntensity += 1;
                        }
                    }
                }
            }
        
            shadowIntensity /= u_FilterSize * u_FilterSize * u_FilterSize;
        }
       
        lighting.diffuse = (1.0f - shadowIntensity) * diffuse * radiance * NdotL;
        lighting.specular = (1.0f - shadowIntensity) * specular * radiance * NdotL;
    }
    else
    {
        lighting.diffuse = albedo;
        lighting.specular = vec3(0.0f);
    }

    return lighting;
}

void main()
{
    vec2 pos = gl_FragCoord.xy / u_ScreenSize;
    
    Lighting lighting = CookTaurance(pos);

    vec2 noiseScale = u_ScreenSize / vec2(u_NoiseSize);

	vec3 position = (u_ViewMatrix * vec4(texture2D(u_Position, pos).xyz, 1.0f)).xyz;
	vec3 normal = (u_NormalMatrix * vec4(texture2D(u_Normal, pos).xyz, 1.0f)).xyz;

	vec3 noise = texture2D(u_Noise, pos * noiseScale).xyz;

	vec3 tangent = normalize(noise - normal * dot(normal, noise));
	vec3 bitangent = cross(normal, tangent);

	mat3 TBN = mat3(tangent, bitangent, normal);

    float samples = 1.0;

	for (int i = 0; i < u_SampleCount && i < MAX_SAMPLES_COUNT; ++i)
	{
		vec3 samplePosition = position + u_Radius * TBN * u_Samples[i];
		
		vec4 screen = u_ProjectionMatrix * vec4(samplePosition, 1.0f);
		screen.xy /= screen.w;
		screen.xy = 0.5f * screen.xy + 0.5f;

		vec3 samplerScreenPosition = (u_ViewMatrix * vec4(texture2D(u_Position, screen.xy).xyz, 1.0f)).xyz;

		float visible = samplePosition.z >= samplerScreenPosition.z + 0.000005f ? 1.0f : 0.0f;
		float distanceCheck = abs(samplePosition.z - samplerScreenPosition.z) < u_Radius ? 1.0f : 0.0f;

		vec3 sampleColor = CookTaurance(screen.xy).diffuse;
		vec3 sampleNormal = (u_NormalMatrix * vec4(texture2D(u_Normal, screen.xy).xyz, 1.0f)).xyz;

		color += visible * distanceCheck * vec4(sampleColor, 1.0f) * max(0.0f, dot(sampleNormal, normal));
        samples += visible * distanceCheck;
	}

    color += vec4(lighting.diffuse + lighting.specular, 1.0f);
	color /= samples; // Solid angle ?
	color.a = 0.0f;
}
