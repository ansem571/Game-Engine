#version 400

out vec4 theFinalColor;	

in vec3 vertexPositionWorld;
in vec3 normalWorld;
in vec3 vertexToFragmentColor;
in vec2 UV;

uniform vec3 lightPositionWorld;
uniform vec3 eyePositionWorld;
uniform vec4 ambientLight;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float applyTexture;
uniform float specularIntensity;

uniform sampler2D myTextureSampler;

void main()
{
	vec4 temp = texture(myTextureSampler, UV);
	vec3 newNormal = vec3((2 * temp) - 1); // goes from (0-1) to (-1 to +1)

	//Diffuse
	vec3 lightVectorWorld = normalize(lightPositionWorld - vertexPositionWorld);
	float brightness = dot(lightVectorWorld, newNormal);
	vec4 diffuseLight = vec4(brightness, brightness, brightness, 1.0);

	//Specular
	vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, newNormal);
	vec3 eyeVectorWorld = normalize(eyePositionWorld - vertexPositionWorld);
	float s = dot(reflectedLightVectorWorld, eyeVectorWorld);
	s = pow(s, specularIntensity);
	vec4 specularLight = vec4(s, s, s, 1.0);
	theFinalColor = temp * (clamp(diffuseLight, 0, 1) + ambientLight) * vec4(diffuseColor, 1.0f) + clamp(specularLight, 0, 1);
}