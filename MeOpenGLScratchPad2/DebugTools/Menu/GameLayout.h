#ifndef GAMELAYOUT_H
#define GAMELAYOUT_H

#include <glm\glm.hpp>
#include <Qt\qboxlayout.h>
#include <QtGui\qlabel>
#include <QtGui\qwidget>

class GameLayout
{
	static GameLayout* instance;
	GameLayout() {}
	GameLayout(const GameLayout&);
	GameLayout& operator=(const GameLayout&);
public:
	QVBoxLayout* fullLayout;
	QVBoxLayout* debugLayout;
	QVBoxLayout* sceneLayout;

	static bool initialize();
	static GameLayout& getInstance() {return *instance;}

	void setupLayouts();
};

#define layoutInfo GameLayout::getInstance()

#endif