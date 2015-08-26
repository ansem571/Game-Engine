#ifndef DEBUG_MENU_H
#define DEBUG_MENU_H
#include <DebugTools\Menu\Infos\WatchInfo.h>
#include <DebugTools\Sliders\SliderInfo.h>
#include <DebugTools\CheckBox\CheckBoxInfo.h>
#include <Qt\qlist.h>
#include <Qt\qtabwidget.h>
class QVBoxLayout;


class DebugMenu
{
	static DebugMenu instance;
	DebugMenu() {}
	DebugMenu(const DebugMenu&);
	DebugMenu& operator=(const DebugMenu&);
public:
	static DebugMenu& getInstance() { return instance;}
	QList<WatchInfo> watchInfos;
	QList<SliderInfo> sliderInfos;
	QList<CheckBoxInfo> checkBoxInfos;

#if DEBUGMENU_ON
	void watch(const char* text, const float& value);
	QVBoxLayout* theLayout;
	QVBoxLayout* otherLayout;

	bool initialize();
	void update();
	void addSlider(const char* text, float min, float max, float val);
	void addCheckBox(const char* text, QWidget* widg);
#else
	void watch(const char* text, const float& value)
	{text; value;}
	QVBoxLayout* theLayout;

	bool initialize(){return true;}
	void update(){}
	void addSlider(const char* text, float min, float max, float val)
	{text; min; max; val;}
	void addCheckBox(const char* text, QWidget* widg)
	{text; widg;}
#endif
};

#define debugMenu DebugMenu::getInstance()

#endif