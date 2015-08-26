#include <DebugTools\Menu\GameLayout.h>

GameLayout* GameLayout::instance = 0;

bool GameLayout::initialize()
{
	if(instance != 0)
		return false;
	instance = new GameLayout();
	return true;
}

void GameLayout::setupLayouts()
{
	fullLayout = new QVBoxLayout();
	debugLayout = new QVBoxLayout();
	sceneLayout = new QVBoxLayout();
}