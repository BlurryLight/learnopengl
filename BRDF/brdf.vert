#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormals;
out vec2 TexCoord;
out vec3 WorldPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
gl_Position = projection * view * model * vec4(aPos,1.0f);
TexCoord = aTexCoord;
WorldPos = vec3(model * vec4(aPos,1.0f));
//mat3 normalMartrix = transpose(inverse(mat3(model)));
//Normal = normalize(normalMartrix * aNormals);
Normal = mat3(model) * aNormals;
}
