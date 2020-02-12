#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in vec3 WorldPos;
in vec3 Normal;

//material parameters
uniform float ao;
uniform vec3 albedo;
uniform float roughness;
uniform float metallic;
//lights
uniform vec3 lightPosition[4];
uniform vec3 lightColors[4];
uniform vec3 camPos;
const float PI = 3.14159265359;

//IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

//N: normal
//H: half-way vector
//roughness: float
float DistributionGGX(vec3 N,vec3 H,float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N,H),0.0);
    float NdotH2 = NdotH * NdotH;

    float numerator = a2;
    float denominator = PI * (NdotH2 * (a2 -1) + 1) * (NdotH2 * (a2 -1) + 1);
    return numerator / max(denominator,0.000000001);//prevent denominator = 0
}
//N:normal
//V:WorldPos to camera
//GeometrySmith composed of
//ndotv and ndotl
float GeometrySchlickGGX(float NdotV,float roughness)
{
 float r = (roughness + 1.0);
 float k =(r*r) / 8.0; //direct light
 float numerator = NdotV;
 float denominator = NdotV * (1.0 -k) + k;
 return numerator / max(denominator,0.001);//prevent denominator = 0
}
//N:normal
//V:...
//L:from light to WorldPos ?
float GeometrySchlick(vec3 N,vec3 V,vec3 L,float roughness)
{
    float NdotV = max(dot(N,V),0.0);
    float NdotL = max(dot(N,L),0.0);
    float ggx2 = GeometrySchlickGGX(NdotV,roughness);
    float ggx1 = GeometrySchlickGGX(NdotL,roughness);
    return ggx1 * ggx2;
}
//cosTheta: ndotv
//F0: reflect ratio at normal incidence
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 +(1.0 - F0) * pow(1.0 - cosTheta,5.0);
}
void main()
{

    vec3 N = normalize(Normal);
    vec3 V = normalize(camPos - WorldPos);//from WorldPos to camPos
    vec3 R = reflect(-V,N);
    ////dielectrics use 0.04, if its metal,mix the albedo with it
    vec3 F0 = vec3(0.04);
    F0 = mix(F0,albedo,metallic);



//reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i=0;i< 4;++i)
    {
        vec3 L = normalize(lightPosition[i] - WorldPos); //from worldpos to light
        vec3 H = normalize(V + L);
        float distance = length(lightPosition[i] - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;

//BRDF
        float NDF = DistributionGGX(N,H,roughness);
        float G = GeometrySchlick(N,V,L,roughness);
        vec3 F = fresnelSchlick(clamp(dot(H,V),0.0,1.0),F0);

        vec3 numerator = NDF * G *F;
        float denominator = 4 * max(dot(N,V),0.0) * max(dot(N,L),0.0);
        vec3 specular = numerator / max(denominator,0.001);

        vec3 KS = F; //ks equals to Fresnel for reflection
        vec3 KD = vec3(1.0) - KS;
        //only non-metal has diffuse lighting
        KD *= 1.0 - metallic; //

        float NdotL = max(dot(N,L),0.0);
        //c = albedo
        Lo += (KD * albedo / PI + specular) * radiance * NdotL;
    }
    //ambient lighting
    vec3 F  = fresnelSchlick(max(dot(N,V),0.0),F0);
    vec3 KS  =  F;
    vec3 KD = 1.0 - KS;
    KD*= 1.0 - metallic;
    vec3 irradiance = texture(irradianceMap,N).rgb;
    vec3 diffuse = irradiance * albedo;
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (KD * diffuse + specular) * ao;


    vec3 color = ambient + Lo;


    //HDR
    color = color / (color + vec3(1.0));
    //gamma correction
    color = pow(color ,vec3(1.0/2.2));

    FragColor = vec4(color,1.0);

}
