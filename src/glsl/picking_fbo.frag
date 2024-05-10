#version 440 core

out vec4 FragColor;
  
uniform uint objectIndex;

void main()
{
    //FragColor = vec3(float(objectIndex), 0, 0);
    //FragColor = vec3(float(100), float(50), float(20));
    FragColor = vec4(objectIndex, 50, 20, 1);
}
