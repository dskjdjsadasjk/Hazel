#type vertex
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

void main()
{
    vs_out.FragPos = aPos;
    vs_out.Normal = aNormal;
    vs_out.TexCoords = aTexCoords;
    gl_Position = u_ViewProjection * model* vec4(aPos, 1.0);
}

#type fragment
#version 450 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;


layout(std140, binding = 1) uniform Light
{
    vec3 u_LightPos;
};

layout(std140, binding = 2) uniform View
{
    vec3 u_ViewPos;
};

uniform float far_plane;

layout (binding = 0) uniform sampler2D texture_diffuse1;
layout (binding = 1) uniform sampler2D texture_specular1;
layout (binding = 2) uniform samplerCube depthMap;

float ShadowCalculation(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - u_LightPos;
    // ise the fragment to light vector to sample from the depth map    
    float closestDepth = texture(depthMap, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    float bias = max(0.05 * (1.0 - dot(normalize(fs_in.Normal), normalize(fragToLight))), 0.005);
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;        
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return shadow;
}

void main()
{           
    vec3 color = texture(texture_diffuse1, fs_in.TexCoords).rgb;
    float distance = length(u_LightPos - fs_in.FragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
    // ambient
    vec3 ambient = 0.05 * color;
    // diffuse
    vec3 lightDir = normalize(u_LightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color * attenuation;
    // specular
    vec3 viewDir = normalize(u_ViewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = texture(texture_specular1, fs_in.TexCoords).rgb * spec * attenuation; // assuming bright white light color
    
    float shadow=ShadowCalculation(fs_in.FragPos);
    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);
    
    FragColor = vec4(lighting, 1.0);

}

