#ifndef CHECKBOX_H
#define CHECKBOX_H

#include <glm\glm.hpp>
#include <QtGui\qcheckbox.h>

class CheckBoxInfo
{
public:
	bool checked;
	QCheckBox* checkBox;
	void initialize(QWidget* widg);
};


#endif