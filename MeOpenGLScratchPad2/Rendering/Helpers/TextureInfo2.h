#ifndef ENGINE_TEXTURE2_INFO_H
#define ENGINE_TEXTURE2_INFO_H
#include <Rendering\RenderTypeDefs.h>

class TextureInfo2
{
public:
	rend_uint TextureID[2];
	rend_uint TextureIndex[2];

	friend class Renderer;
	TextureInfo2() {}
};
#endif