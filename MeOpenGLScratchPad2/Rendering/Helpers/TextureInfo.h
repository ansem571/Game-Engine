#ifndef ENGINE_TEXTURE_INFO_H
#define ENGINE_TEXTURE_INFO_H
#include <Rendering\RenderTypeDefs.h>

class TextureInfo
{
public:
	rend_uint TextureID[2];
	rend_uint TextureIndex[2];
	bool singleTexture;

	friend class Renderer;
	TextureInfo() {}
};
#endif