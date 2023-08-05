#version 330 core
out vec4 FragColor;
in vec3 cPos;
void main()
{
   FragColor = vec4(cPos, 1.0f);
}