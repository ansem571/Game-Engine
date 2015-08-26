#include <DebugTools\Sliders\SliderInfo.h>

void SliderInfo::initialize(float min, float max, float value)
{
	slider = new DebugSlider(min, max, true, 500.0f);
	currentValue = value;
	slider->setValue(currentValue);
}

