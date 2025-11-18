#type vertex
#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

void main()
{
    TexCoords = aTexCoords;    
    gl_Position = u_ViewProjection * model * vec4(aPos, 1.0);
}

#type fragment
#version 450 core

out vec4 FragColor;

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D texture_diffuse1;
layout (binding = 1) uniform sampler2D texture_specular1;

void main()
{    
    FragColor = texture(texture_diffuse1, TexCoords);
}

