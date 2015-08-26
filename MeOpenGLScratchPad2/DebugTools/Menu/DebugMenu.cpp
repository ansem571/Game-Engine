#include <DebugTools\Menu\DebugMenu.h>
#if DEBUGMENU_ON
#include <Qt\qlabel.h>
#include <QtGui\qhboxlayout>
#include <QtGui\qvboxlayout>
#include <Qt\qdebug.h>

DebugMenu DebugMenu::instance;

bool DebugMenu::initialize()
{
	theLayout = new QVBoxLayout();
	otherLayout = new QVBoxLayout();
	return true;
}

void DebugMenu::watch(const char* text, const float& value)
{
	QHBoxLayout* thisRow;

	otherLayout->addLayout(thisRow = new QHBoxLayout);
	thisRow->setSpacing(0);
	thisRow->setMargin(0);

	thisRow->addWidget(new QLabel(text));

	WatchInfo w;
	thisRow->addWidget(w.theLabel = new QLabel());
	w.theFloat = &value;
	watchInfos.append(w);
}

void DebugMenu::update()
{
	for (int i = 0; i < watchInfos.size(); i++)
	{
		WatchInfo& w = watchInfos[i];
		w.theLabel->setText(QString::number(*(w.theFloat), 'g', 2));
	}

	//loop sliderInfos
	for (int i = 0; i < sliderInfos.size(); i++)
	{
		SliderInfo& s = sliderInfos[i];
		s.currentValue = s.slider->value();
		s.slider->setValue(s.currentValue);
	}

	//loop checkBoxInfos
	for (int i = 0; i < checkBoxInfos.size(); i++)
	{
		CheckBoxInfo& c = checkBoxInfos[i];
		c.checked = c.checkBox->isChecked();
		c.checkBox->setChecked(c.checked);
	}
}

void DebugMenu::addSlider(const char* text, float min, float max, float val)
{
	QHBoxLayout* thisRow;

	theLayout->addLayout(thisRow = new QHBoxLayout);
	thisRow->setSpacing(0);
	thisRow->setMargin(0);

	thisRow->addWidget(new QLabel(text));

	SliderInfo s;
	s.initialize(min, max, val);

	thisRow->addWidget(s.slider);

	sliderInfos.append(s);
}

void DebugMenu::addCheckBox(const char* text, QWidget* widg)
{
	QHBoxLayout* thisRow;

	otherLayout->addLayout(thisRow = new QHBoxLayout);
	thisRow->addWidget(new QLabel(text));

	CheckBoxInfo c;
	c.initialize(widg);

	thisRow->addWidget(c.checkBox);

	checkBoxInfos.append(c);
}
#else

DebugMenu DebugMenu::instance;

#endif