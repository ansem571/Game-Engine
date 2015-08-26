#ifndef GEMOETRY_H
#define GEOMETRY_H
#include <GL\glew.h>
#include <Rendering\RenderTypeDefs.h>
class VertexLayoutInfo;

class Geometry
{
public:
	bool isAvailable;
	
	rend_uint vertexArrayObjectID;

	rend_uint indexMode;
	rend_uint numIndices;
	rend_uint indexByteOffset;
	rend_uint indiceDataType;

	friend class Renderer;
public:
	Geometry() : isAvailable(true)	{}
};

#endif