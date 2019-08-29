#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;


out vec2 TexCoord;
uniform mat4 v;
uniform mat4 m;
uniform mat4 p;
void main()
{
    gl_Position = p * v * m * vec4(aPos.x,aPos.y,aPos.z,1.0);
    TexCoord = aTexCoord;
}
