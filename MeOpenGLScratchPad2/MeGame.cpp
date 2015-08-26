#include <MeGame.h>
#include <cassert>
#include <IO\FileIO.h>
#include <glm\gtx\transform.hpp>
#include <Qt\qapplication.h>
#include <Qt\qdebug.h>
#include <DebugTools\Menu\GameLayout.h>
#include <DebugTools\Menu\DebugMenu.h>
#include <QtCore\qtimer.h>

float *fps = new float;

bool MeGame::initialize()
{
	if(!renderer.initialize())
		return false;
	setupLayout();
	doRendererStuff();
	clock.initialize();
	clock.newFrame();
	connect(&pumper, SIGNAL(timeout()), this, SLOT(newFrame()));
	return true;
}

void MeGame::runGameLoop()
{
	pumper.start(0);
}

bool MeGame::shutDown()
{
	bool ret = true;
	ret &= renderer.shutdown();
	pumper.stop();
	return ret;
}

void MeGame::newFrame()
{
	clock.newFrame();
	updateCamera();
	checkDebugMenu();
	debugMenu.update();

#if DEBUGMENU_ON
	
	thePass->x = debugMenu.sliderInfos[0].currentValue;
	thePass->y = debugMenu.sliderInfos[1].currentValue;
	thePass->z = debugMenu.sliderInfos[2].currentValue;

	thePass->freq = debugMenu.sliderInfos[3].currentValue;

	//thePass->applyDiffuse = debugMenu.checkBoxInfos[0].checked;

	//thePass->spec = debugMenu.sliderInfos[3].currentValue;
#endif

	renderer.draw();
	*fps = 1/clock.timeElapsedLastFrame();
}

/*void MeGame::signState()
{
	//initialize the signs to be false no matter what
	thePass->elementary = false;
	thePass->middle = false;
	thePass->highSchool = false;
	thePass->college = false;

	//check to see if we looped back to beginning
	if(timePassed >= changeFrame)
	{
		timePassed -= changeFrame;
		currentSign++;
		if(currentSign == 4)
		{
			currentSign = 0;
		}
	}

	//elementary
	if(currentSign == 0)
		thePass->elementary = true;

	//middle school
	if(currentSign == 1)
		thePass->middle = true;

	//high school
	if(currentSign == 2)
		thePass->highSchool = true;

	//college
	if(currentSign == 3)
		thePass->college = true;
}*/


void MeGame::setupLayout()
{
	mainWindow = new QWidget();
	debugWidgets = new QTabWidget();
	sliders = new QWidget();
	others = new QWidget();
	
	//can I setup the layouts
	if(!layoutInfo.initialize())
		return;

	//initialize the different vbox/hbox layouts
	layoutInfo.setupLayouts();

	//did I initialize the debugMenu
	if(!debugMenu.initialize())
		return;
#if DEBUGMENU_ON
	// look into making the layout a widget
	debugMenu.theLayout->setSpacing(0);
	debugMenu.theLayout->setMargin(0);
	sliders->setLayout(debugMenu.theLayout);
	sliders->setContentsMargins(10, 10, 10, 10);

	debugWidgets->addTab(sliders, "Sliders"); // make this sliders

	others->setLayout(debugMenu.otherLayout);
	others->setContentsMargins(10, 10, 10, 10);

	debugWidgets->addTab(others, "Other Controls"); //make this watches and checkboxes
	//this will work

#endif
	layoutInfo.sceneLayout->addWidget(&renderer);
	layoutInfo.debugLayout->addWidget(debugWidgets);

#if DEBUGMENU_ON
	
	layoutInfo.fullLayout->addLayout(layoutInfo.debugLayout);

#endif
	
	layoutInfo.fullLayout->addLayout(layoutInfo.sceneLayout);
	
	mainWindow->setLayout(layoutInfo.fullLayout);

	*fps = clock.timeElapsedLastFrame();
	
	//HIPPO


	debugMenu.watch("FPS", *fps);
	//debugMenu.addCheckBox("Apply Diffuse Map", others);
	//debugMenu.addCheckBox("Apply Normal Map", others);
	//debugMenu.addCheckBox("Apply Ambient Map", others);

	mainWindow->show();
}

ShapeData MeGame::readMyBinaryFile(char* fileName)
{
	std::ifstream input(fileName,
		std::ios::binary | std::ios::in);

	input.seekg(0, std::ios::end);
	unsigned int numBytes = input.tellg();
	input.seekg(0, std::ios::beg);
	char* bytes = new char[numBytes];
	input.read(bytes, numBytes);
	input.close();

	ShapeData ret;

	char* metaData = bytes;
	char* vertexBase = bytes + 2 * sizeof(unsigned int);

	ret.numVerts = *reinterpret_cast<unsigned int*>(metaData);
	ret.numIndices = reinterpret_cast<unsigned int*>(metaData)[1];
	
	char* indexBase = vertexBase + ret.vertexBufferSize();
	ret.verts = reinterpret_cast<Vertex*>(vertexBase);
	ret.indices = reinterpret_cast<unsigned short*>(indexBase);

	return ret;

}

void MeGame::doRendererStuff()
{
	rend_uint sizes[] = {3, 4, 3, 2};
	rend_uint normalSizes[] = {3, 4, 3, 3, 2};
	ShapeData sphere = ShapeGenerator::makeSphere(20);
	ShapeData teddy = readMyBinaryFile("Assets\\HisBinaryFile.bin");
	ShapeData torus = ShapeGenerator::makeTorus(20);
	ShapeData lightSphere = ShapeGenerator::makeSphere(20);
	ShapeData plane = ShapeGenerator::makePlane(8);
	ShapeData cube = makeCube();

	//vertexlayout
	VertexLayoutInfo* vertexLayoutInfo = 
		renderer.addVertexLayoutInfo(sizes, ARRAYSIZE(sizes), Vertex::STRIDE);
	
	//geo
	Geometry* sphereGeo;
	sphereGeo = renderer.addGeometry(
		sphere.verts, sphere.vertexBufferSize(),
		sphere.indices, sphere.numIndices, sphere.indexBufferSize(), DataType::DT_UNSIGNED_SHORT,
		GL_TRIANGLES, vertexLayoutInfo);

	Geometry* teddyGeo = renderer.addGeometry(
		teddy.verts, teddy.vertexBufferSize(),
		teddy.indices, teddy.numIndices, teddy.indexBufferSize(), DT_UNSIGNED_SHORT,
		GL_TRIANGLES, vertexLayoutInfo);

	Geometry* torusGeo = renderer.addGeometry(
		torus.verts, torus.vertexBufferSize(),
		torus.indices, torus.numIndices, torus.indexBufferSize(), DT_UNSIGNED_SHORT,
		GL_TRIANGLES, vertexLayoutInfo);

	Geometry* lightSphereGeo;
	lightSphereGeo = renderer.addGeometry(
		lightSphere.verts, lightSphere.vertexBufferSize(),
		lightSphere.indices, lightSphere.numIndices, lightSphere.indexBufferSize(), DataType::DT_UNSIGNED_SHORT,
		GL_TRIANGLES, vertexLayoutInfo);

	Geometry* planeGeo;
	planeGeo = renderer.addGeometry(
		plane.verts, plane.vertexBufferSize(),
		plane.indices, plane.numIndices, plane.indexBufferSize(), DataType::DT_UNSIGNED_SHORT,
		GL_TRIANGLES, vertexLayoutInfo);

	Geometry* cubeGeo;
	cubeGeo = renderer.addGeometry(
		cube.verts, cube.vertexBufferSize(),
		cube.indices, cube.numIndices, cube.indexBufferSize(), DataType::DT_UNSIGNED_SHORT,
		GL_TRIANGLES, vertexLayoutInfo);

	//shaders
	//standard shader files
	std::string v = (FileIO::file2String("ShaderCode\\VertexShaderPassThroughCode.glsl"));
	std::string f = (FileIO::file2String("ShaderCode\\FragmentShaderPassThroughCode.glsl"));
	//light shader files
	std::string lv = (FileIO::file2String("ShaderCode\\VertexShaderCode.glsl"));
	std::string lf = (FileIO::file2String("ShaderCode\\FragmentShaderCode.glsl"));
	//normal map light shader files
	std::string nlv = (FileIO::file2String("ShaderCode\\NormalVertexShaderCode.glsl"));
	std::string nlf = (FileIO::file2String("ShaderCode\\NormalFragmentShaderCode.glsl"));
	//light Source shader files
	std::string lsv = (FileIO::file2String("ShaderCode\\LightSourceVertexShaderCode.glsl"));
	std::string lsf = (FileIO::file2String("ShaderCode\\LightSourceFragmentShaderCode.glsl"));
	//cube map shader files
	std::string clv = (FileIO::file2String("ShaderCode\\TangentVertexShaderCode.glsl"));
	std::string clf = (FileIO::file2String("ShaderCode\\TangentFragmentShaderCode.glsl"));
	//multi texture shader
	std::string mtsv = (FileIO::file2String("ShaderCode\\MultiTextureVertexShaderCode.glsl"));
	std::string mstf = (FileIO::file2String("ShaderCode\\MultiTextureFragmentShaderCode.glsl"));
	//alpha texture shader
	std::string atv = (FileIO::file2String("ShaderCode\\alphaVertexShaderCode.glsl"));
	std::string atf = (FileIO::file2String("ShaderCode\\alphaFragmentShaderCode.glsl"));

	
	//shader info
	ShaderInfo* normalShader = renderer.addShader(v.c_str(), f.c_str());
	ShaderInfo* lightShader = renderer.addShader(lv.c_str(), lf.c_str());
	ShaderInfo* normalLightShader = renderer.addShader(nlv.c_str(), nlf.c_str());
	ShaderInfo* lightSourceShader = renderer.addShader(lsv.c_str(), lsf.c_str());
	ShaderInfo* tangentShader = renderer.addShader(clv.c_str(), clf.c_str());
	ShaderInfo* multiTextureShader = renderer.addShader(mtsv.c_str(), mstf.c_str());
	ShaderInfo* alphaTextureShader = renderer.addShader(atv.c_str(), atf.c_str());


	//texture info
	TextureInfo* sphereTexture = renderer.addTexture("/Assets/space.png", 0);
	TextureInfo* cubeTexture = renderer.addTexture("/Assets/ToonTeddyBear.png", 1);
	TextureInfo* planeTexture = renderer.addTexture("/Assets/Shapes.png", 2);
	TextureInfo* spaceTexture = renderer.addTexture("/Assets/space.png", 3);
	TextureInfo* multiTexture1 = renderer.addTexture("/Assets/Leaf.png", "/Assets/whiteLeaf.png", 4, 5);
	TextureInfo* glassTexture = renderer.addTexture("/Assets/GlassTexture", 6);
	TextureInfo* noiseTexture = renderer.addNoiseTexture(7, 20);

	//add a pass
	thePass = renderer.addPassInfo();

	//transform the renderable
	glm::mat4 finalTransform;
	finalTransform = glm::translate(0.0f, 0.0f, 0.0f) * glm::scale(0.1f, 0.1f, 0.1f);

	//light source 0
	Renderable* renderable = renderer.addRenderable(sphereTexture, lightSphereGeo, lightSourceShader, finalTransform);
	thePass->addRenderable(renderable);

	//noise texture 1
	finalTransform = glm::translate(0.0f, 5.0f, -5.0f) * glm::rotate(90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	renderable = renderer.addRenderable(noiseTexture, planeGeo, normalLightShader, finalTransform);
	thePass->addRenderable(renderable);

	//HIPPO
	debugMenu.addSlider("Light X Position:", -20.0f, +20.0f, 0.0f); 
	debugMenu.addSlider("Light Y Position:", -20.0f, +20.0f, 5.0f); 
	debugMenu.addSlider("Light Z Position:", -20.0f, +20.0f, 0.0f); 

	debugMenu.addSlider("Octave Frequency: ", 0.0f, 100.0f, 5.0f);
}


ShapeData MeGame::makeCube()
{
using glm::vec2;
using glm::vec3;
using glm::vec4;

Vertex stackVerts[] = 
{
// Top
vec3(-1.0f, +1.0f, +1.0f), // 0
vec4(+1.0f, +0.0f, +0.0f, +1.0f), // Tangent
vec3(+0.0f, +1.0f, +0.0f), // Normal
vec2(+0.0f, +1.0f), // UV

vec3(+1.0f, +1.0f, +1.0f), // 1
vec4(+1.0f, +0.0f, +0.0f, +1.0f), // Tangent
vec3(+0.0f, +1.0f, +0.0f), // Normal
vec2(+1.0f, +1.0f), // UV

vec3(+1.0f, +1.0f, -1.0f), // 2
vec4(+1.0f, +0.0f, +0.0f, +1.0f), // Tangent
vec3(+0.0f, +1.0f, +0.0f), // Normal
vec2(+1.0f, +0.0f), // UV

vec3(-1.0f, +1.0f, -1.0f), // 3
vec4(+1.0f, +0.0f, +0.0f, +1.0f), // Tangent
vec3(+0.0f, +1.0f, +0.0f), // Normal
vec2(+0.0f, +0.0f), // UV

// Front
vec3(-1.0f, +1.0f, -1.0f), // 4
vec4(+1.0f, +0.0f, +0.0f, +1.0f), // Tangent
vec3(+0.0f, +0.0f, -1.0f), // Normal
vec2(+0.0f, +1.0f), // UV

vec3(+1.0f, +1.0f, -1.0f), // 5
vec4(+1.0f, +0.0f, +0.0f, +1.0f), // Tangent
vec3(+0.0f, +0.0f, -1.0f), // Normal
vec2(+1.0f, +1.0f), // UV

vec3(+1.0f, -1.0f, -1.0f), // 6
vec4(+1.0f, +0.0f, +0.0f, +1.0f), // Tangent
vec3(+0.0f, +0.0f, -1.0f), // Normal
vec2(+1.0f, +0.0f), // UV

vec3(-1.0f, -1.0f, -1.0f), // 7
vec4(+1.0f, +0.0f, +0.0f, +1.0f), // Tangent
vec3(+0.0f, +0.0f, -1.0f), // Normal
vec2(+0.0f, +0.0f), // UV

// Right
vec3(+1.0f, +1.0f, -1.0f), // 8
vec4(+0.0f, 0.0f, -1.0f, +1.0f), // Tangent
vec3(+1.0f, +0.0f, +0.0f), // Normal
vec2(+1.0f, +0.0f), // UV

vec3(+1.0f, +1.0f, +1.0f), // 9
vec4(+0.0f, 0.0f, -1.0f, +1.0f), // Tangent
vec3(+1.0f, +0.0f, +0.0f), // Normal
vec2(+0.0f, +0.0f), // UV

vec3(+1.0f, -1.0f, +1.0f), // 10
vec4(+0.0f, 0.0f, -1.0f, +1.0f), // Tangent
vec3(+1.0f, +0.0f, +0.0f), // Normal
vec2(+0.0f, +1.0f), // UV

vec3(+1.0f, -1.0f, -1.0f), // 11
vec4(+0.0f, 0.0f, -1.0f, +1.0f), // Tangent
vec3(+1.0f, +0.0f, +0.0f), // Normal
vec2(+1.0f, +1.0f), // UV

// Left
vec3(-1.0f, +1.0f, +1.0f), // 12
vec4(+0.0f, 0.0f, +1.0f, +1.0f), // Tangent
vec3(-1.0f, +0.0f, +0.0f), // Normal
vec2(+1.0f, +0.0f), // UV

vec3(-1.0f, +1.0f, -1.0f), // 13
vec4(+0.0f, 0.0f, +1.0f, +1.0f), // Tangent
vec3(-1.0f, +0.0f, +0.0f), // Normal
vec2(+0.0f, +0.0f), // UV

vec3(-1.0f, -1.0f, -1.0f), // 14
vec4(+0.0f, 0.0f, +1.0f, +1.0f), // Tangent
vec3(-1.0f, +0.0f, +0.0f), // Normal
vec2(+0.0f, +1.0f), // UV

vec3(-1.0f, -1.0f, +1.0f), // 15
vec4(+0.0f, 0.0f, +1.0f, +1.0f), // Tangent
vec3(-1.0f, +0.0f, +0.0f), // Normal
vec2(+1.0f, +1.0f), // UV

// Back
vec3(+1.0f, +1.0f, +1.0f), // 16
vec4(+1.0f, +0.0f, +0.0f, +1.0f), // Tangent
vec3(+0.0f, +0.0f, +1.0f), // Normal
vec2(+1.0f, +0.0f), // UV

vec3(-1.0f, +1.0f, +1.0f), // 17
vec4(+1.0f, +0.0f, +0.0f, +1.0f), // Tangent
vec3(+0.0f, +0.0f, +1.0f), // Normal
vec2(+0.0f, +0.0f), // UV

vec3(-1.0f, -1.0f, +1.0f), // 18
vec4(+1.0f, +0.0f, +0.0f, +1.0f), // Tangent
vec3(+0.0f, +0.0f, +1.0f), // Normal
vec2(+0.0f, +1.0f), // UV

vec3(+1.0f, -1.0f, +1.0f), // 19
vec4(+1.0f, +0.0f, +0.0f, +1.0f), // Tangent
vec3(+0.0f, +0.0f, +1.0f), // Normal
vec2(+1.0f, +1.0f), // UV

// Bottom
vec3(+1.0f, -1.0f, -1.0f), // 20
vec4(+1.0f, +0.0f, +0.0f, +1.0f), // Tangent
vec3(+0.0f, -1.0f, +0.0f), // Normal
vec2(+1.0f, +1.0f), // UV

vec3(-1.0f, -1.0f, -1.0f), // 21
vec4(+1.0f, +0.0f, +0.0f, +1.0f), // Tangent
vec3(+0.0f, -1.0f, +0.0f), // Normal
vec2(+0.0f, +1.0f), // UV

vec3(-1.0f, -1.0f, +1.0f), // 22
vec4(+1.0f, +0.0f, +0.0f, +1.0f), // Tangent
vec3(+0.0f, -1.0f, +0.0f), // Normal
vec2(+0.0f, +0.0f), // UV

vec3(+1.0f, -1.0f, +1.0f), // 23
vec4(+1.0f, +0.0f, +0.0f, +1.0f), // Tangent
vec3(+0.0f, -1.0f, +0.0f), // Normal
vec2(+1.0f, +0.0f), // UV
};

unsigned short stackIndices[] = {
0, 1, 2, 0, 2, 3, // Top
4, 5, 6, 4, 6, 7, // Front
8, 9, 10, 8, 10, 11, // Right 
12, 13, 14, 12, 14, 15, // Left
16, 17, 18, 16, 18, 19, // Back
20, 22, 21, 20, 23, 22, // Bottom
};

return copyToShapeData(stackVerts, ARRAYSIZE(stackVerts), stackIndices, ARRAYSIZE(stackIndices));
}

ShapeData MeGame::copyToShapeData(Vertex* stackVerts, rend_uint sizeVerts, unsigned short* stackIndices, rend_uint sizeIndices)
{
	ShapeData ret;

	ret.numVerts = sizeVerts;
	ret.numIndices = sizeIndices;
	ret.verts = new Vertex[sizeVerts];
	ret.indices = new ushort[sizeIndices];
	for(uint i = 0; i < sizeVerts; i++)
	{
		ret.verts[i] = stackVerts[i];
	}

	for(uint i = 0; i < sizeIndices; i++)
	{
		ret.indices[i] = stackIndices[i];
	}
	return ret;
}

void MeGame::checkDebugMenu()
{
#if DEBUGMENU_ON
	if(GetAsyncKeyState(VK_TAB))
	{
		if(debugWidgets->isVisible())
			debugWidgets->setVisible(false);
		else
			debugWidgets->setVisible(true);
		QEventLoop loop;
		QTimer::singleShot(1000, &loop, SLOT(quit()));
		loop.exec();
	}
#endif
}

void MeGame::updateCamera()
{
	updateCameraFromMouse();
	updateCameraFromKeyboard();
}

void MeGame::updateCameraFromMouse()
{
	if((QApplication::mouseButtons() & Qt::MouseButton::LeftButton) == 0)
		return;
	QPoint p = renderer.mapFromGlobal(QCursor::pos());
	if(p.x() < 0 || p.y() < 0 || p.x() > renderer.width() || p.y() > renderer.height())
		return;
	thePass->camera.mouseUpdate(glm::vec2(p.x(), p.y()));
}

void MeGame::updateCameraFromKeyboard()
{
	Camera& camera = thePass->camera;
	if(GetAsyncKeyState('W')){
		camera.moveForward();
	}
	if(GetAsyncKeyState('A')){
		camera.strafeLeft();
	}
	if(GetAsyncKeyState('S')){
		camera.moveBackward();
	}
	if(GetAsyncKeyState('D')){
		camera.strafeRight();
	}
	if(GetAsyncKeyState('R')){
		camera.moveUp();
	}
	if(GetAsyncKeyState('F')){
		camera.moveDown();
	}
}