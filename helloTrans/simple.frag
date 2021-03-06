#version 330 core
out vec4 FragColor;
in vec3 vertex_color;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float visiable;
void main()
{
    FragColor = mix(texture(texture1,TexCoord),texture(texture2,vec2(1 - TexCoord.x,1-TexCoord.y)),visiable);
}
