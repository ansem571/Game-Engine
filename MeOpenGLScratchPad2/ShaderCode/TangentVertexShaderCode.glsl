#version 400
	
in layout(location=0) vec3 vertexPositionModel;
in layout(location=1) vec3 tangentVector;
in layout(location=2) vec3 normalModel;
in layout(location=3) vec2 vertexUV;

uniform mat4 modelToWorldMatrix;
uniform mat4 modelToProjectionMatrix;

out mat3 normalWorld;
out vec3 vertexPositionWorld;
out vec2 UV;

void main()
{
	vec3 biTangent = cross(tangentVector, normalModel);
	vec4 v = vec4(vertexPositionModel, 1.0);
	gl_Position = modelToProjectionMatrix * v;
	
	vec3 tangent = vec3(modelToWorldMatrix * vec4(tangentVector, 0));
	vec3 bi = vec3(modelToWorldMatrix * vec4(biTangent, 0));
	vec3 norm = vec3(modelToWorldMatrix * vec4(normalModel, 0));

	normalWorld = mat3(tangent, bi, norm);
	vertexPositionWorld = vec3(modelToWorldMatrix * v);

	UV = vertexUV;
}