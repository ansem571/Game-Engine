#ifndef ENGINE_PARAMETER_TYPE_H
#define ENGINE_PARAMETER_TYPE_H

enum ParameterType
{
	PT_BOOL = sizeof(bool) * 1,
	PT_FLOAT = sizeof(float) * 1,
	PT_VEC2 = sizeof(float) * 2,
	PT_VEC3 = sizeof(float) * 3,
	PT_VEC4 = sizeof(float) * 4,
	PT_MAT3 = sizeof(float) * 9,
	PT_MAT4 = sizeof(float) * 16,
	PT_INT,
	PT_TEXTURE,
};

#endif