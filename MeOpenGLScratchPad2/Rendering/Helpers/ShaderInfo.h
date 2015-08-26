#pragma once

#include <Rendering\RenderTypeDefs.h>

class ShaderInfo
{
	bool isAvailable;
	rend_uint programID;
	rend_uint numLayouts;
	friend class Renderer;
public:
	ShaderInfo(): isAvailable(true), programID(-1) {}
};

