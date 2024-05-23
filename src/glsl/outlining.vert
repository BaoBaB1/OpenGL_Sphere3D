#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * vec4(aPos + aNormal * 0.04, 1.0);
}
