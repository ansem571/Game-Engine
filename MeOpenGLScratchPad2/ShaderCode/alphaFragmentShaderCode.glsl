#version 420

out vec4 theFinalColor;	

in vec3 normalWorld;
in vec3 vertexPositionWorld;
in vec3 vertexToFragmentColor;
in vec2 UV;

uniform vec3 lightPositionWorld;
uniform vec3 eyePositionWorld;
uniform vec4 ambientLight;
uniform vec3 diffuseColor;
uniform vec3 specularColor;

uniform sampler2D myTextureSampler;

vec3 phongModel (vec3 position, vec3 norm)
{
	//Diffuse
	vec3 lightVectorWorld = normalize(lightPositionWorld - vertexPositionWorld);
	float brightness = dot(lightVectorWorld, normalize(norm));
	vec4 diffuseLight = vec4(brightness, brightness, brightness, 1.0);

	//Specular
	vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, norm);
	vec3 eyeVectorWorld = normalize(eyePositionWorld - position);
	float s = dot(reflectedLightVectorWorld, eyeVectorWorld);
	s = pow(s, 6);
	vec4 specularLight = vec4(s, s, s, 1.0);

	return vec3((clamp(diffuseLight, 0, 1) + ambientLight) * vec4(diffuseColor, 1.0) + clamp(specularLight, 0, 1) * vec4(specularColor, 1.0));
}

void main()
{
	vec4 temp = texture(myTextureSampler, UV);
	float alpha = 0.5f;

	if(gl_FrontFacing)
		theFinalColor = vec4(phongModel(vertexPositionWorld, normalWorld), 1.0) * vec4(temp.rgb, alpha);
	else
		theFinalColor = vec4(phongModel(vertexPositionWorld, -normalWorld), 1.0) * vec4(temp.rgb, alpha);
}