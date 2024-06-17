#version 450

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

void main() 
{
    outColor = vec4(0.5 * (inNormal + vec3(1.0,1.0,1.0)), 1.0);
}