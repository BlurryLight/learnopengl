#version 330 core
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

uniform vec3 viewPos;
in vec3 FragPos;
in vec3 Normal;
void main()
{
    //ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    //diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm,lightDir),0.0);
    vec3 diffuse = diff * lightColor;
    //specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);//note the -
    float spec = pow(max(dot(viewDir,reflectDir),0.0),64);
    vec3 specular = specularStrength * spec * lightColor;
    //32 is magic number to describe shininess
    //do not touch it


    FragColor = vec4(objectColor * (ambient + diffuse + specular) ,1.0);
}
