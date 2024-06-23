#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec4 color;

void main()
{
    gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * vec4(aPos, 1.0);
    color = aColor;
}
