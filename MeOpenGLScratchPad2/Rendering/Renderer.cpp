#include <GL\glew.h>
#include <Rendering\Renderer.h>
#include <Qt\qdebug.h>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\transform.hpp>
#include <Qt\qdir.h>

Renderer* Renderer::instance = 0;

bool Renderer::initialize()
{
	if (instance != 0)
		//TODO:: Log
		return false;
	instance = new Renderer();
	instance->setMinimumSize(800, 600);
	return true;
}

void Renderer::initializeGL()
{
	glewInit();
	numRenderables = 0;
	numPassInfos = 0;
	glEnable(GL_DEPTH_TEST);
}

bool Renderer::shutdown()
{
	if (instance == 0)
		//TODO: Log
		return false;
	delete instance;
	instance = 0;
	return true;
}

//TODO: vertex array

Geometry* Renderer::addGeometry(
	void* verts, GLuint vertexDataSize,
	void* indices, GLuint numIndices, GLuint indexBufferSize, DataType indiceDataType,
	GLuint indexingMode, VertexLayoutInfo* vertexLayoutInfo)
{
	Geometry* ret = findAvailableGeometry();

	BufferInfo* vertexBuffer = getAvailableBuffer(vertexDataSize);
	sendDataToBuffer(vertexBuffer, vertexDataSize, verts);
	GLuint vertexDataBufferByteOffset = vertexBuffer->nextAvailableByte;
	vertexBuffer->nextAvailableByte += vertexDataSize;

	BufferInfo* indexBuffer = getAvailableBuffer(indexBufferSize);
	sendDataToBuffer(indexBuffer, indexBufferSize, indices);
	ret->indexByteOffset = indexBuffer->nextAvailableByte;
	indexBuffer->nextAvailableByte += indexBufferSize;

	ret->vertexArrayObjectID = setupVertexArrayObject(
		vertexLayoutInfo, vertexBuffer->glBufferID,
		vertexDataBufferByteOffset, indexBuffer->glBufferID);

	ret->indexMode = indexingMode;
	ret->isAvailable = false;
	ret->numIndices = numIndices;
	ret->indiceDataType = indiceDataType == DT_UNSIGNED_SHORT ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;

	return ret;
}

Geometry* Renderer::findAvailableGeometry()
{
	for (unsigned int i = 0; i < MAX_GEOMETRIES; i++)
	{
		if (geometries[i].isAvailable)
		{
			return geometries + i;
		}
	}
	qDebug() << "Error, failed to add geometry. NO empty geometry slots available...";
	exit(1);
}

GLuint Renderer::setupVertexArrayObject(
	VertexLayoutInfo* vertexLayoutInfo,
	GLuint vertexBufferID,
	GLuint vertexBufferDataOffset,
	GLuint indexBufferID)
{
	GLuint ret;
	glGenVertexArrays(1, &ret);
	glBindVertexArray(ret);
	GLuint currentOffset = 0;
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	for (rend_uint j = 0; j < vertexLayoutInfo->numAttributes; j++)
	{
		glEnableVertexAttribArray(j);
		glVertexAttribPointer(j, vertexLayoutInfo->attributeSizes[j],
			GL_FLOAT, GL_FALSE, vertexLayoutInfo->stride,
			(void*)(vertexBufferDataOffset + currentOffset));
		currentOffset += vertexLayoutInfo->attributeSizes[j] * sizeof(float);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBindVertexArray(0);
	return ret;
}

void Renderer::sendDataToBuffer(BufferInfo* buffer, GLsizeiptr size, void* data)
{
	glBindBuffer(GL_ARRAY_BUFFER, buffer->glBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, buffer->nextAvailableByte, size, data);
}

BufferInfo* Renderer::allocateNewBuffer()
{
	BufferInfo* b = findUnallocatedBuffer();
	glGenBuffers(1, &b->glBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, b->glBufferID);
	glBufferData(GL_ARRAY_BUFFER, BufferInfo::MAX_BUFFER_SIZE, 0, GL_DYNAMIC_DRAW);
	b->hasBuffer = true;
	b->nextAvailableByte = 0;
	return b;
}

BufferInfo* Renderer::findUnallocatedBuffer()
{
	for (unsigned int i = 0; i < MAX_BUFFER_INFOS; i++)
	{
		if (!bufferInfos[i].hasBuffer)
			return bufferInfos + i;
	}
	qDebug() << "Error, failed to find available buffer...";
	exit(1);
	return 0;
}

BufferInfo* Renderer::findBufferWithSpace(rend_uint neededSize)
{
	for (unsigned int i = 0; i < MAX_BUFFER_INFOS; i++)
		if (bufferInfos[i].hasBuffer && bufferInfos[i].getRemainingSize() > neededSize)
			return bufferInfos + i;
	return 0;
}

BufferInfo* Renderer::getAvailableBuffer(GLsizeiptr neededSize)
{
	BufferInfo* ret = findBufferWithSpace(neededSize);
	if (ret != 0)
		return ret;
	return allocateNewBuffer();
}

ShaderInfo* Renderer::addShader(
	const char* vertexShaderFilePath,
	const char* fragmentShaderFilePath)
{
	ShaderInfo* ret = getAvailableShaderInfo();

	rend_uint vertexShaderID = compileShader(vertexShaderFilePath, GL_VERTEX_SHADER);
	rend_uint fragmentShaderID = compileShader(fragmentShaderFilePath, GL_FRAGMENT_SHADER);

	checkCompileStatus(vertexShaderID);
	checkCompileStatus(fragmentShaderID);

	ret->programID = linkProgram(vertexShaderID, fragmentShaderID);

	checkLinkStatus(ret->programID);

	return ret;

}

ShaderInfo* Renderer::getAvailableShaderInfo()
{
	static int currentSlot = 0;

	int i = currentSlot;

	if (i < MAX_SHADER_INFOS)
	{
		currentSlot++;
		return &shaderinfos[i];
	}

	qDebug() << "Error. No available shader info slots...";
	exit(1);
	return 0;
}

Renderable* Renderer::addRenderable(
	TextureInfo* texture,
	Geometry* what,
	ShaderInfo* how,
	const glm::mat4& where) {
	Renderable* ret = getAvailableRenderable();

	ret->t = texture;
	ret->geometry = what;
	ret->shader = how;
	ret->transformations = where;

	return ret;
}

Renderable* Renderer::getAvailableRenderable()
{
	if (numRenderables < MAX_RENDERABLES)
		return renderables + numRenderables++;

	qDebug() << "Error. No available renderable slots...";
	exit(1);
	return 0;
}

rend_uint Renderer::compileShader(const char* code, GLenum shaderType)
{
	rend_uint ret = glCreateShader(shaderType);

	const char* adaptor[1];
	adaptor[0] = code;
	glShaderSource(ret, 1, adaptor, 0);
	glCompileShader(ret);

	return ret;
}

void Renderer::checkCompileStatus(rend_uint shaderID)
{
	GLint compileStatus;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus == GL_TRUE)
	{
		return;
	}
	GLint logLength;
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
	GLint shaderType;
	glGetShaderiv(shaderID, GL_SHADER_TYPE, &shaderType);
	char* buffer = new char[logLength];
	GLsizei bitBucket;
	glGetShaderInfoLog(shaderID, logLength, &bitBucket, buffer);
	qDebug() << (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader compile error..." << buffer;
	delete[] buffer;
	exit(0);
}

GLuint Renderer::linkProgram(rend_uint vertexShaderID, rend_uint fragmentShaderID)
{
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);

	glLinkProgram(programID);
	return programID;
}

void Renderer::checkLinkStatus(rend_uint programID)
{
	GLint compileStatus;
	glGetProgramiv(programID, GL_LINK_STATUS, &compileStatus);

	if (compileStatus == GL_TRUE)
	{
		return;
	}
	GLint logLength;
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);
	char* buffer = new char[logLength];
	GLsizei bitBucket;
	glGetProgramInfoLog(programID, logLength, &bitBucket, buffer);
	qDebug() << "Program failed to link..." << buffer;
	delete[] buffer;
	exit(0);
}

VertexLayoutInfo* Renderer::addVertexLayoutInfo(
	rend_uint* sizes, rend_uint numAttributes, rend_uint stride)
{
	assert(numAttributes <= VertexLayoutInfo::MAX_VERTEX_ATTRIBUTES);
	VertexLayoutInfo* ret = getAvailableVertexLayoutInfo();
	memcpy(ret->attributeSizes, sizes, numAttributes * sizeof(*sizes));
	ret->numAttributes = numAttributes;
	ret->stride = stride;
	ret->isAvailable = false;
	return ret;
}

VertexLayoutInfo* Renderer::getAvailableVertexLayoutInfo()
{
	//VertexLayoutInfo* ret = 0;
	for (rend_uint i = 0; i < MAX_VERTEX_LAYOUT_INFOS; i++)
		if (vertexLayoutInfos[i].isAvailable)
			return vertexLayoutInfos + i;
	assert(false); //TODO: Replace all asserts with custom assert
	return 0;
}

//look at myTextureSampler for understanding of where to go
void Renderer::setupUniforms(Renderable* renderable, PassInfo* passInfo, rend_uint i)
{
	//matrix setup
	glm::mat4 perspective = glm::perspective(60.0f, (float)width() / height(), 0.1f, 100.0f);
	glm::mat4 worldToViewMatrix = passInfo->camera.getWorldToViewMatrix();
	glm::mat4 worldToProjectMatrix = perspective * worldToViewMatrix;

	//vector setup
	glm::vec3 eyePosition = passInfo->camera.getPosition();
	glm::vec4 ambientLight(glm::vec3(0.05f, 0.05f, 0.05f), 1.0f);
	glm::vec3 diffuseLight(glm::vec3(0.5f, 0.5f, 0.5f));
	glm::vec3 lightPosition = glm::vec3(passInfo->x, passInfo->y, passInfo->z);
	glm::vec3 specularLightColor = glm::vec3(0.5f, 0.5f, 0.5f);

	//light uniform locations
	GLint eyePositionWorldUniformLocation = glGetUniformLocation(renderable->shader->programID, "eyePositionWorld");
	GLint lightPositionUniformLocation = glGetUniformLocation(renderable->shader->programID, "lightPositionWorld");
	GLint ambientLightUniformLocation = glGetUniformLocation(renderable->shader->programID, "ambientLight");
	GLint diffuseLightColorUniformLocation = glGetUniformLocation(renderable->shader->programID, "diffuseColor");
	GLint specularColorUniformLocation = glGetUniformLocation(renderable->shader->programID, "specularColor");

	//texture uniform locations
	GLint textureUniformLocation = glGetUniformLocation(renderable->shader->programID, "myTextureSampler");
	GLint textureUniformLocation2 = glGetUniformLocation(renderable->shader->programID, "myTextureSampler2");

	//matrix uniform locations
	GLint mvwUniformLocation = glGetUniformLocation(renderable->shader->programID, "modelToWorldMatrix");
	GLint mvpUniformLocation = glGetUniformLocation(renderable->shader->programID, "modelToProjectionMatrix");

	//activate textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderable->t->TextureID[0]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, renderable->t->TextureID[1]);

	//define mvp
	glm::mat4 mvp =
		renderable->transformations;

	glm::mat4 finalTransform = worldToProjectMatrix * mvp;

	//set the uniforms to their respective values
	if (textureUniformLocation != -1)
	{
		glUniform1i(textureUniformLocation, 0);
		//qDebug() << "texture";
	}
	if (textureUniformLocation2 != -1)
	{
		glUniform1i(textureUniformLocation2, 1);
		//qDebug() << "texture";
	}
	if (eyePositionWorldUniformLocation != -1)
	{
		glUniform3fv(eyePositionWorldUniformLocation, 1, &eyePosition[0]);
		//qDebug() << "eye position";
	}
	if (lightPositionUniformLocation != -1)
	{
		glUniform3fv(lightPositionUniformLocation, 1, &lightPosition[0]);
		//qDebug() << "light position";
	}
	if (ambientLightUniformLocation != -1)
	{
		glUniform4fv(ambientLightUniformLocation, 1, &ambientLight[0]);
		//qDebug() << "ambient light";
	}
	if (diffuseLightColorUniformLocation != -1)
	{
		glUniform3fv(diffuseLightColorUniformLocation, 1, &diffuseLight[0]);
		//qDebug() << "diffuse color";
	}
	if (specularColorUniformLocation != -1)
	{
		glUniform3fv(specularColorUniformLocation, 1, &specularLightColor[0]);
		//qDebug() << "specular color";
	}

	if (mvwUniformLocation != -1)
	{
		glUniformMatrix4fv(mvwUniformLocation, 1, GL_FALSE, &mvp[0][0]);
		//qDebug() << "mvp";
	}
	if (mvpUniformLocation != -1)
	{
		glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &finalTransform[0][0]);
		//qDebug() << "mvp";
	}
}
//single texture
void Renderer::setupConventionalUniforms(Renderable* renderable, PassInfo* passInfo, rend_uint i)
{
	//matrix setup
	glm::mat4 perspective = glm::perspective(60.0f, (float)width() / height(), 0.1f, 100.0f);
	glm::mat4 worldToViewMatrix = passInfo->camera.getWorldToViewMatrix();
	glm::mat4 worldToProjectMatrix = perspective * worldToViewMatrix;

	//vector setup
	glm::vec3 eyePosition = passInfo->camera.getPosition();
	glm::vec4 ambientLight(glm::vec3(0.05f, 0.05f, 0.05f), 1.0f);
	glm::vec3 diffuseLight(glm::vec3(0.5f, 0.5f, 0.5f));
	glm::vec3 lightPosition = glm::vec3(passInfo->x, passInfo->y, passInfo->z);
	//for lab 4
	//otherwise
	glm::vec3 specularLightColor = glm::vec3(0.5f, 0.5f, 0.5f);
	float specularIntensity = passInfo->spec;

	//light uniform locations
	GLint eyePositionWorldUniformLocation = glGetUniformLocation(renderable->shader->programID, "eyePositionWorld");
	GLint lightPositionUniformLocation = glGetUniformLocation(renderable->shader->programID, "lightPositionWorld");
	GLint ambientLightUniformLocation = glGetUniformLocation(renderable->shader->programID, "ambientLight");
	GLint diffuseLightColorUniformLocation = glGetUniformLocation(renderable->shader->programID, "diffuseColor");
	GLint specularColorUniformLocation = glGetUniformLocation(renderable->shader->programID, "specularColor");
	GLint specularIntensityUniformLocation = glGetUniformLocation(renderable->shader->programID, "specularIntensity");

	//texture uniform locations
	GLint applyTextureUniformLocation = glGetUniformLocation(renderable->shader->programID, "applyTexture");
	GLint textureUniformLocation = glGetUniformLocation(renderable->shader->programID, "myTextureSampler");

	//matrix uniform locations
	GLint mvwUniformLocation = glGetUniformLocation(renderable->shader->programID, "modelToWorldMatrix");
	GLint mvpUniformLocation = glGetUniformLocation(renderable->shader->programID, "modelToProjectionMatrix");

	//activate textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderable->t->TextureID[0]);

	//apply texture
	float applyTexture = 0.0f;
	if (renderable->t->TextureIndex[0] == 1 || renderable->t->TextureIndex[0] == 2 || renderable->t->TextureIndex[0] == 3)
	{
		applyTexture = 1.0f;
	}

	//define mvp
	glm::mat4 mvp =
		renderable->transformations;

	glm::mat4 finalTransform = worldToProjectMatrix * mvp;

	//set the uniforms to their respective values
	if (textureUniformLocation != -1)
	{
		glUniform1i(textureUniformLocation, 0);
		//qDebug() << "texture";
	}
	if (applyTextureUniformLocation != -1)
	{
		glUniform1f(applyTextureUniformLocation, applyTexture);
		//qDebug() << "apply texture";
	}
	if (eyePositionWorldUniformLocation != -1)
	{
		glUniform3fv(eyePositionWorldUniformLocation, 1, &eyePosition[0]);
		//qDebug() << "eye position";
	}
	if (lightPositionUniformLocation != -1)
	{
		glUniform3fv(lightPositionUniformLocation, 1, &lightPosition[0]);
		//qDebug() << "light position";
	}
	if (ambientLightUniformLocation != -1)
	{
		glUniform4fv(ambientLightUniformLocation, 1, &ambientLight[0]);
		//qDebug() << "ambient light";
	}
	if (diffuseLightColorUniformLocation != -1)
	{
		glUniform3fv(diffuseLightColorUniformLocation, 1, &diffuseLight[0]);
		//qDebug() << "diffuse color";
	}
	if (specularColorUniformLocation != -1)
	{
		glUniform3fv(specularColorUniformLocation, 1, &specularLightColor[0]);
		//qDebug() << "specular color";
	}
	if (specularIntensityUniformLocation != -1)
	{
		glUniform1f(specularIntensityUniformLocation, specularIntensity);
	}

	if (mvwUniformLocation != -1)
	{
		glUniformMatrix4fv(mvwUniformLocation, 1, GL_FALSE, &mvp[0][0]);
		//qDebug() << "mvp";
	}
	if (mvpUniformLocation != -1)
	{
		glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &finalTransform[0][0]);
		//qDebug() << "mvp";
	}
}

void Renderer::paintGL()
{
	for (rend_uint i = 0; i < numPassInfos; i++)
		doPass(passInfos + i);
}

PassInfo* Renderer::addPassInfo()
{
	assert(numPassInfos < MAX_PASS_INFOS);
	return passInfos + numPassInfos++;
}

void Renderer::doPass(PassInfo* passInfo)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width(), height());

	int start = 0;
	for (rend_uint i = 0; i < passInfo->numRenderables; i++)
	{
		//qDebug() << "object " << (i+1);
		Renderable* r = renderables + i;

		if (!r->visible) continue;

		Geometry* g = r->geometry;

		glUseProgram(r->shader->programID);

		glBindVertexArray(g->vertexArrayObjectID);

#if DEBUGMENU_ON
		if (i == 0)
		{
			r->transformations = glm::translate(
				passInfo->x,
				passInfo->y,
				passInfo->z)
				* glm::scale(0.1f, 0.1f, 0.1f);
		}

		if (i == 1)
		{
			r->transformations = glm::translate(
				passInfo->CubeX,
				passInfo->CubeY,
				passInfo->CubeZ)
				* glm::rotate(passInfo->RotateX, glm::vec3(1.0f, 0.0f, 0.0f))
				* glm::rotate(passInfo->RotateY, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		/*if (i == 3)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}*/


#endif
		//setupUniforms(r);
		if(r->t->singleTexture)
			setupConventionalUniforms(r, passInfo, i);
		else
			setupUniforms(r, passInfo, i);
		glDrawElements(
			g->indexMode, g->numIndices,
			g->indiceDataType, (void*)(g->indexByteOffset));
		//glDisable(GL_BLEND);
	}
	//qDebug() << "Finished pass";
}

void Renderer::draw()
{
	repaint();
}

TextureInfo* Renderer::addTexture(char* const file, rend_uint index)
{
	TextureInfo* t = new TextureInfo();
	glGenTextures(1, &t->TextureID[0]);
	glBindTexture(GL_TEXTURE_2D, t->TextureID[0]);

	std::string currentPath = QDir::currentPath().toLocal8Bit().data();
	std::string imgPath = file;
	std::string filename = currentPath + imgPath;
	QImage image = QGLWidget::convertToGLFormat(QImage(filename.c_str(), "PNG"));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());

	t->TextureIndex[0] = index;
	t->singleTexture = true;
	return t;
}

TextureInfo* Renderer::addTexture(char* const file1, char* const file2, rend_uint index, rend_uint index2)
{
	TextureInfo* t = new TextureInfo();
	glGenTextures(2, t->TextureID);

	std::string currentPath = QDir::currentPath().toLocal8Bit().data();
	std::string imgPath = file1;
	std::string filename = currentPath + imgPath;
	QImage image = QGLWidget::convertToGLFormat(QImage(filename.c_str(), "PNG"));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, t->TextureID[0]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, image.width(), image.height());
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width(), image.height(), GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	t->TextureIndex[0] = index;

	std::string currentPath2 = QDir::currentPath().toLocal8Bit().data();
	std::string imgPath2 = file2;
	std::string filename2 = currentPath2 + imgPath2;
	QImage image2 = QGLWidget::convertToGLFormat(QImage(filename2.c_str(), "PNG"));

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, t->TextureID[1]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, image2.width(), image2.height());
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image2.width(), image2.height(), GL_RGBA, GL_UNSIGNED_BYTE, image2.bits());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	t->TextureIndex[1] = index2;
	t->singleTexture = false;
	return t;
}