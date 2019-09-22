#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

uniform Material material;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

uniform vec3 viewPos;
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
void main()
{
    //ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords));
    //diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm,lightDir),0.0);
    vec3 diffuse = vec3(texture(material.diffuse, TexCoords)) * light.diffuse * diff;
    //specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);//note the -
    float spec = pow(max(dot(viewDir,reflectDir),0.0), material.shininess);
    vec3 specular = vec3(texture(material.specular,TexCoords)) * spec * light.specular;
    //32 is magic number to describe shininess
    //do not touch it

    vec3 emission = texture(material.emission,TexCoords).rgb;
    FragColor = vec4(ambient + diffuse + specular + emission,1.0f);
//    FragColor = vec4( emission,1.0f);
}
