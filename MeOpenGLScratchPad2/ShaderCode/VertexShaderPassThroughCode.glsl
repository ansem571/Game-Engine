#version 430

in layout(location = 0) vec3 vertexPositionModel;
in layout(location = 1) vec4 vertexColor;
in layout(location = 2) vec3 normalModel;
in layout(location = 3) vec2 vertexUV;

out vec3 vertexToFragmentColor;
out vec2 UV;

uniform mat4 modelToWorldMatrix;
uniform mat4 modelToProjectionMatrix;

void main()
{
	gl_Position = modelToProjectionMatrix * vec4(vertexPositionModel, 1.0f);
	vertexToFragmentColor = vec3(vertexColor);

	UV = vertexUV;
}