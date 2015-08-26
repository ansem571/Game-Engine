#ifndef SLIDERINFO_H
#define SLIDERINFO_H

#include <glm\glm.hpp>
#include <DebugTools\DebugSlider.h>

class SliderInfo
{
public:
	float currentValue;
	DebugSlider* slider;
	void initialize(float min, float max, float value);
};


#endif