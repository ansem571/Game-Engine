#include <DebugTools\CheckBox\CheckBoxInfo.h>


void CheckBoxInfo::initialize(QWidget* widg)
{
	checkBox = new QCheckBox(widg);
	checked = true;
}



