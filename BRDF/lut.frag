#version 330 core
out vec4 FragColor;
uniform sampler2D lut;
in vec2 TexCoords;
void main()
{
    FragColor = vec4(texture(lut,TexCoords).rgb,1.0);
}
