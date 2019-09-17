#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat3 normal_m;
uniform mat4 v;
uniform mat4 m;
uniform mat4 p;
void main()
{

    FragPos = vec3(m * vec4(aPos,1.0));
    Normal = normal_m * aNormal;
    gl_Position = p * v * vec4(FragPos,1.0);
}
