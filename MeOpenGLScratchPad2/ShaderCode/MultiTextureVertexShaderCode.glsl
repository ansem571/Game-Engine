#version 400

in layout(location = 0) vec3 VertexPosition;
in layout(location = 1) vec3 VertexNormal;
in layout(location = 2) vec2 VertexTexCoord;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main()
{
	TexCoord = VertexTexCoord;
	Normal = normalize(NormalMatrix * VertexNormal);
	Position = vec3(ModelViewMatrix * vec4(VertexPosition, 1.0);
	gl_Position = MVP * vec4(VertexPosition, 1.0;
}