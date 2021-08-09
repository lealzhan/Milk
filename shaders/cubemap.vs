#version 400 compatibility
layout (location = 0) in vec3 position;//aPos;

out vec3 WorldPos;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    WorldPos = position;
    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}