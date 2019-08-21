#version 330 core
out vec4 FragColor;
in vec3 vertex_color;
in vec2 TexCoord;

uniform sampler2D ourTexture;
void main()
{
    FragColor = texture(ourTexture,TexCoord);
}