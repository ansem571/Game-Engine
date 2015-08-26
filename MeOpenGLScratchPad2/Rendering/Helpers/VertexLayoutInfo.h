#ifndef VERTEX_LAYOUT_INFO_H
#define VERTEX_LAYOUT_INFO_H

#include <GL\glew.h>

class VertexLayoutInfo
{
	static const GLuint MAX_VERTEX_ATTRIBUTES = 5;
	GLuint attributeSizes[MAX_VERTEX_ATTRIBUTES];
	GLuint numAttributes;
	GLuint stride;
	bool isAvailable;
	friend class Renderer;

public:
	VertexLayoutInfo() : stride(0), numAttributes(0), isAvailable(true){}
};

#endif
