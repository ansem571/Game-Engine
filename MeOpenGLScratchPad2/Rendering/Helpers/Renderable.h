#ifndef ENGINE_RENDERABLE_H
#define ENGINE_RENDERABLE_H

class Geometry;
class ShaderInfo;
class TextureInfo;
#include <glm\glm.hpp>


class Renderable
{
public:
	Geometry* geometry;
	ShaderInfo* shader;
	glm::mat4 transformations;
	TextureInfo* t;
	bool visible;
	Renderable() : visible(true){}
};

#endif