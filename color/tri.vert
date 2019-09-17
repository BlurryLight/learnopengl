#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 LightingColor;

uniform vec3 lightColor;
uniform vec3 lightPos;

uniform vec3 viewPos;

uniform mat3 normal_m;
uniform mat4 v;
uniform mat4 m;
uniform mat4 p;
void main()
{

    gl_Position = p * v * m  * vec4(aPos,1.0);
    //
    vec3 Position = vec3(m * vec4(aPos,1.0));
    vec3 Normal = normal_m * aNormal;

    //ambient
    float ambientStrength = 0.1;
    vec3 ambient  = ambientStrength * lightColor;
    //diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - Position);
    float diff = max(dot(norm,lightDir),0.0);
    vec3 diffuse = diff * lightColor;

    //specular
    float specularStrengh = 1.0;
    vec3 viewDir = normalize(viewPos - Position);
    vec3 reflectDir = reflect(-lightDir,viewDir);
    float spec = pow(max(dot(reflectDir,viewDir),0.0),32);
    vec3 specular = specularStrengh * spec * lightColor;

    LightingColor = ambient + diffuse + specular;

}
