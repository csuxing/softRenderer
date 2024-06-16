#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec4 inJoint;
layout(location = 4) in vec4 inWeight;

layout(location = 0) out vec3 outNormal;
layout(location = 1) out vec2 outTexCoord;

layout(set = 0, binding = 0) uniform UniformBufferObject 
{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main() 
{
    gl_Position = ubo.proj * ubo.view * vec4(inPosition, 1.0);
    outNormal = inNormal;
    outTexCoord = inTexCoord;
}