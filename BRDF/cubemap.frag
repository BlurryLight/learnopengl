#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform sampler2D equirectangularMap;
const vec2 invAtan = vec2(0.1591, 0.3183); //1/2pi 1/pi

vec2 SamplerSphericalMap(vec3 v) //map sphere to uv
{
    vec2 uv = vec2(atan(v.z,v.x),asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
 vec2 uv = SamplerSphericalMap(normalize(WorldPos));
 vec3 color = texture(equirectangularMap,uv).rgb;
 FragColor = vec4(color,1.0);
}


