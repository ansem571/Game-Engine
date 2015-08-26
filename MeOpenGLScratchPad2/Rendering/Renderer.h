#ifndef RENDERER_H
#define RENDERER_H
#include <QtOpenGL\qglwidget>
#include <Rendering\Helpers\Renderable.h>
#include <Rendering\Helpers\Geometry.h>
#include <Rendering\Helpers\BufferInfo.h>
#include <Rendering\Helpers\ShaderInfo.h>
#include <Rendering\Helpers\VertexLayoutInfo.h>
#include <Rendering\Helpers\DataType.h>
#include <Rendering\Helpers\PassInfo.h>
#include <Rendering\Helpers\TextureInfo.h>

class Geometry;

class Renderer : public QGLWidget
{
	static const unsigned int MAX_VERTEX_LAYOUT_INFOS = 10;
	VertexLayoutInfo vertexLayoutInfos[MAX_VERTEX_LAYOUT_INFOS];
	static const unsigned int MAX_GEOMETRIES = 20;
	Geometry geometries[MAX_GEOMETRIES];
	static const unsigned int MAX_RENDERABLES = 100;
	Renderable renderables[MAX_RENDERABLES];
	static const unsigned int MAX_BUFFER_INFOS = 10;
	BufferInfo bufferInfos[MAX_BUFFER_INFOS];
	static const unsigned int MAX_SHADER_INFOS = 10;
	ShaderInfo shaderinfos[MAX_SHADER_INFOS];
	static const unsigned int MAX_PASS_INFOS = 100;
	PassInfo passInfos[MAX_PASS_INFOS];
	uint numPassInfos;
	uint numRenderables;

	//helper functions for addGeometry()
	Geometry* findAvailableGeometry();

	BufferInfo* getAvailableBuffer(GLsizeiptr neededSize);
	void sendDataToBuffer(BufferInfo* buffer, GLsizeiptr size, void* data);
	BufferInfo* findBufferWithSpace(rend_uint neededSize);
	BufferInfo* findUnallocatedBuffer();
	BufferInfo* allocateNewBuffer();

	//helper functions for addShader()
	ShaderInfo* getAvailableShaderInfo();

	//helper functions for addRenderable()
	Renderable* getAvailableRenderable();
	VertexLayoutInfo* getAvailableVertexLayoutInfo();

	rend_uint compileShader(const char* code, GLenum shaderType);
	void checkCompileStatus(rend_uint ShaderID);
	GLuint linkProgram(rend_uint vertexShaderID, rend_uint fragmentShaderID);
	void checkLinkStatus(rend_uint programID);


	static Renderer* instance;
	Renderer() {}
	Renderer(const Renderer&);
	Renderer& operator=(const Renderer&);

	void setupUniforms(Renderable* renderable, PassInfo* passInfo, rend_uint i);
	void setupConventionalUniforms(Renderable* renderable, PassInfo* passInfo, rend_uint i);
	GLuint setupVertexArrayObject(
		VertexLayoutInfo* vertexLayoutInfo,
		GLuint vertexBufferID,
		GLuint vertexBufferDataOffset,
		GLuint indexBufferID);
protected:
	void initializeGL();
	void paintGL();
public:
	static bool initialize();
	static bool shutdown();
	PassInfo* addPassInfo();
	void doPass(PassInfo* passInfo);
	void draw();

	Geometry* addGeometry(
		void* verts, GLuint vertexDataSize,
		void* indices, GLuint numIndices, GLuint indexBufferSize, DataType type,
		GLuint indexingMode, VertexLayoutInfo* vertexLayoutInfo);

	ShaderInfo* addShader(
		const char* vertexShaderID,
		const char* fragmentShaderID);

	Renderable* addRenderable(
		TextureInfo* texture,
		Geometry* what,
		ShaderInfo* how,
		const glm::mat4& where = glm::mat4());

	VertexLayoutInfo* addVertexLayoutInfo(
		rend_uint* sizes,
		rend_uint numAttributes,
		rend_uint stride);

	TextureInfo* addNoiseTexture(rend_uint index, float freq);
	TextureInfo* addTexture(char* const file, rend_uint index);
	TextureInfo* addTexture(char* const file1, char* const file2, rend_uint index, rend_uint index2);
	GLubyte * generateData(float freq);
	static Renderer& getInstance() { return *instance; }
};

#define renderer Renderer::getInstance()

#endif