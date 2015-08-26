#ifndef ENGINE_PASS_INFO_H
#define ENGINE_PASS_INFO_H
#include <Rendering\RenderTypeDefs.h>
#include <Camera\Camera.h>
class Renderable;

class PassInfo
{
	static const rend_uint MAX_RENDERABLES = 50;
	Renderable* renderables[MAX_RENDERABLES];
	rend_uint numRenderables;
	friend class Renderer;

public:
	Camera camera;
	float x, y, z,
		CubeX, CubeY, CubeZ,
		RotateX, RotateY, spec;
	PassInfo() : numRenderables(0) {}
	inline void addRenderable(Renderable* renderable);
};

void PassInfo::addRenderable(Renderable* renderable)
{
	assert(numRenderables < MAX_RENDERABLES);
	renderables[numRenderables++] = renderable;
}

#endif