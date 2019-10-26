#version 330 core
layout (location = 0) in vec3 aPos;

layout (std140) uniform Matrices
{
    mat4 p;
    mat4 v;
};


uniform mat4 m;
void main()
{

    gl_Position = p * v *m* vec4(aPos,1.0);
}
