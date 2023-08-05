#version 330 core
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aColor;
out vec3 cPos;
uniform float offset;
uniform float moveX;
uniform float moveY;
uniform float moveZ;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 trueColor;
void main()
{
    gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    cPos = aColor;
    cPos = trueColor;
}