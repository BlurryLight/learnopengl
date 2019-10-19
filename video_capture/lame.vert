#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 v;
uniform mat4 m;
uniform mat4 p;
void main()
{
    gl_Position = p *  m * vec4(aPos.x,aPos.y,aPos.z,1.0);
}