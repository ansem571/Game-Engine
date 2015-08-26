#include <Qt\qapplication.h>
#include <MeGame.h>

void main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	MeGame meGame;

	
	if(!meGame.initialize())
		return;

	meGame.runGameLoop();

	app.exec();
	meGame.shutDown();
}