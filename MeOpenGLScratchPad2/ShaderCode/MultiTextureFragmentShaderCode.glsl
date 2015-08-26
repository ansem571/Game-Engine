in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout(binding=0) uniform sampler2D tex1;
layout(binding=1) uniform sampler2D tex2;

struct LightInfo
{
	vec4 Position; // light position in eye coords.
	vec3 Intensity; // A, D, S intensity
};

uniform LightInfo Light;

struct MaterialInfo
{
	vec3 Ka; // Ambient reflectivity
	vec3 Kd; // Diffuse reflectivity
	vec3 Ks; // Specular reflect
	float Shininess; // Specular shininess factor
};

uniform MaterialInfo Material;

out vec4 FragColor;

void phongModel(vec3 pos, vec3 norm, out vec3 ambAndDiff, out vec3 spec)
{

}