#version 400
	
in layout(location=0) vec3 vertexPositionModel;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normalModel;
in layout(location=3) vec2 vertexUV;

uniform mat4 modelToWorldMatrix;
uniform mat4 modelToProjectionMatrix;

out vec3 normalWorld;
out vec3 vertexPositionWorld;
out vec3 vertexToFragmentColor;
out vec2 UV;

void main()
{
	vec4 v = vec4(vertexPositionModel, 1.0);
	gl_Position = modelToProjectionMatrix * v;
	normalWorld = vec3(modelToWorldMatrix * vec4(normalModel, 0));
	vertexPositionWorld = vec3(modelToWorldMatrix * v);

	vertexToFragmentColor = vec3(vertexColor);
	UV = vertexUV;
}