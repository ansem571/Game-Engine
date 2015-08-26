#version 430

in vec3 vertexToFragmentColor;
in vec2 UV;

out vec4 daColor;

uniform sampler2D myTextureSampler;
uniform float applyTexture;

void main()
{

	vec3 temp = texture(myTextureSampler, UV).rgb;
	if(applyTexture == 1.0f)
	{
		daColor = vec4(temp, 1.0f);
	}
	else
	{
		daColor = vec4(vertexToFragmentColor, 1.0f);
	}
}