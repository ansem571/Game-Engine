#ifndef Game_ME_GAME_H
#define Game_ME_GAME_H
#include <GL\glew.h>
#include <Qt\qobject.h>
#include <Qt\qtimer.h>
#include <Clock\Clock.h>
#include <Rendering\Renderer.h>
#include <Qt\qtest.h>
#include <ShapeGenerator.h>
#include <Qt\qtabwidget.h>
//#include <NewVertex\NormalVertex.h>

using Neumont::ShapeData;
using Neumont::ShapeGenerator;
using Neumont::Vertex;

class PassInfo;
class NormalVertex;
using Timing::Clock;

class MeGame: public QObject
{
	Q_OBJECT;

	QTimer pumper;
	Clock clock;

	void setupLayout();
	ShapeData readMyBinaryFile(char* fileName);
	void doRendererStuff();
	void checkDebugMenu();
	void updateCamera();
	void updateCameraFromMouse();
	void updateCameraFromKeyboard();
	//void signState();

	PassInfo* thePass;
	private slots:
		void newFrame();
public:
	float yes;
	QWidget* mainWindow;
	QTabWidget* debugWidgets;
	QWidget* sliders;
	QWidget* others;
	bool initialize();
	void runGameLoop();
	bool shutDown();
	ShapeData makeCube();
	ShapeData copyToShapeData(Vertex* stackVerts, rend_uint sizeVerts, unsigned short* stackIndices, rend_uint sizeIndices);

};

#endif