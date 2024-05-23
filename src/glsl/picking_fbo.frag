#version 440 core

out vec4 FragColor;
  
uniform uint objectIndex;

void main()
{
    FragColor = vec4(objectIndex, 50, 20, 1);
}
