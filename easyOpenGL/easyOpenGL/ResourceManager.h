#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "rapidxml.hpp"
#include "Vertex.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

using namespace rapidxml;
using namespace std;

GLenum load_texture_type(char * const &type);
GLenum load_texture_filter(char * const &filter);
GLenum load_texture_wrap(char * const & wrap);
enum file_type
{
	obj_file = 1,
	obj_file_no_texture
};
class ModelResource
{
private:
	friend class Model;
	char* path;
	file_type type;
public:
	ModelResource(char * const &_path, file_type const &_type):path(_path), type(_type){}
	ModelResource() :path(NULL) { cout << "Constructor implicit apelat - ModelResource: Undefined behaviour.\n"; }
	~ModelResource() { delete[] path; }
};

class Model
{
protected:
	ModelResource* modelResPtr; 
	GLuint vboId; //Vertex Buffer Object Id
	GLuint nr_vertices;
public:
	Model(ModelResource * const &_modelResPtr) :modelResPtr(_modelResPtr) {}
	Model() { cout << "Constructor implicit apelat - Model: Undefined behaviour.\n"; }
	virtual void Load();
	virtual void Bind() {
		glBindBuffer(GL_ARRAY_BUFFER, vboId);
	}
	virtual void Draw()
	{
		
		glDrawArrays(GL_TRIANGLES, 0, nr_vertices);			
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
	}
	virtual void DrawInstanced(GLuint const &nr_instances)
	{
		glDrawArraysInstanced(GL_TRIANGLES, 0, nr_vertices, nr_instances);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	~Model() {
		glDeleteBuffers(1, &vboId);
	}
};

class TextureResource
{
private:
	friend class Texture;
	GLenum type;
	char* path;
	GLenum min_filter, mag_filter;
	GLenum wrap_s, wrap_t;
public:
	TextureResource(GLenum const &_type, char * const &_path, GLenum const &_min_filter,
		GLenum const &_mag_filter, GLenum const &_wrap_s, GLenum const &_wrap_t):
	type(_type), path(_path), min_filter(_min_filter), mag_filter(_mag_filter), wrap_s(_wrap_s), wrap_t(_wrap_t){}
	TextureResource(){ cout << "Constructor implicit apelat -TextureResource: Undefined behaviour.\n"; }
	~TextureResource() { delete[] path; }
};

class Texture
{
private:
	TextureResource* textureResPtr;
	GLuint textureId;
public:
	Texture(TextureResource * const &_textureResPtr) :textureResPtr(_textureResPtr) {};
	Texture(){ cout << "Constructor implicit apelat - Texture: Undefined behaviour.\n"; }
	void Load();
	void Bind(int const &i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(textureResPtr->type, textureId);
	}
	~Texture() { glDeleteTextures(1, &textureId); };
};

class ShaderResource
{
private:
	friend class Shader;
	char* vertexShader;
	char* fragmentShader;
public:
	ShaderResource(char * const &_vs, char * const &_fs) :vertexShader(_vs), fragmentShader(_fs) {}
	ShaderResource(){ cout << "Constructor implicit apelat -ShaderResource: Undefined behaviour.\n"; }
	~ShaderResource() { delete[] vertexShader; delete[] fragmentShader; }
};

class Shader
{
protected:
	ShaderResource* shaderResPtr;
	GLuint vertexShaderId, fragmentShaderId, programId;

	GLint positionAttribute, colorAttribute, 
		normAttribute, binormAttribute, tgtAttribute,
		uvAttribute, uvBlendAttribute,
		transfMatrixUniform,
		texture1Uniform, texture2Uniform, texture3Uniform, texture4Uniform,
		textureBlendUniform,
		modelMatrixUniform,
		cameraPosUniform,
		objectColorUniform,lightColorUniform, lightPosUniform, viewPosUniform,
		transfMatrixInstanceAttribute,
	    modelMatrixInstanceAttribute;
		;

	GLuint LoadShader(GLenum type, char * filename)
	{
		GLuint shader;
		GLint compiled;

		// Create the shader object
		shader = glCreateShader(type);

		if (shader == 0)
			return 0;

		// Load the shader source
		FILE * pf;
		if (fopen_s(&pf, filename, "rb") != 0)
			return NULL;
		fseek(pf, 0, SEEK_END);
		long size = ftell(pf);
		fseek(pf, 0, SEEK_SET);

		char * shaderSrc = new char[size + 1];
		fread(shaderSrc, sizeof(char), size, pf);
		shaderSrc[size] = 0;
		fclose(pf);

		glShaderSource(shader, 1, (const char **)&shaderSrc, NULL);
		delete[] shaderSrc;

		// Compile the shader
		glCompileShader(shader);

		// Check the compile status
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

		if (!compiled)
		{
			GLint infoLen = 0;

			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

			if (infoLen > 1)
			{
				char* infoLog = new char[infoLen];


				glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
				printf("Error compiling shader <%s>:\n%s\n", filename, infoLog);

				delete[] infoLog;
			}

			glDeleteShader(shader);
			return 0;
		}

		return shader;
	}
	GLuint LoadProgram(GLuint vertexShader, GLuint fragmentShader)
	{
		GLuint programObject;
		GLint linked;

		// Create the program object
		programObject = glCreateProgram();

		if (programObject == 0)
			return 0;

		glAttachShader(programObject, vertexShader);
		glAttachShader(programObject, fragmentShader);

		// Link the program
		glLinkProgram(programObject);

		// Check the link status
		glGetProgramiv(programObject, GL_LINK_STATUS, &linked);

		if (!linked)
		{
			GLint infoLen = 0;

			glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);

			if (infoLen > 1)
			{
				char* infoLog = new char[sizeof(char) * infoLen];


				glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
				printf("Error linking program:\n%s\n", infoLog);

				delete infoLog;
			}

			glDeleteProgram(programObject);
			return 0;
		}

		return programObject;
	}
public:	
	Shader(ShaderResource * const &_shaderResPtr) :shaderResPtr(_shaderResPtr) {};
	Shader() { cout << "Constructor implicit apelat - Shader: Undefined behaviour.\n"; };
	void Load();
	void Use() {
		glUseProgram(programId);
		if (positionAttribute != -1)
		glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
		if (colorAttribute != -1)
		glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
		if (normAttribute != -1)
		glVertexAttribPointer(normAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, norm));
		if (binormAttribute != -1)
		glVertexAttribPointer(binormAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, binorm));
		if (tgtAttribute != -1)
		glVertexAttribPointer(tgtAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tgt));
		if (uvAttribute != -1)
		glVertexAttribPointer(uvAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
		if (uvBlendAttribute != -1)
		glVertexAttribPointer(uvBlendAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uvBlend));

		if (positionAttribute != -1)glEnableVertexAttribArray(positionAttribute); //glVertexAttribDivisor(positionAttribute, 0);
		if (colorAttribute != -1)glEnableVertexAttribArray(colorAttribute); //glVertexAttribDivisor(colorAttribute, 0);
		if (normAttribute != -1)glEnableVertexAttribArray(normAttribute); //glVertexAttribDivisor(normAttribute, 0);
		if (binormAttribute != -1)glEnableVertexAttribArray(binormAttribute);// glVertexAttribDivisor(binormAttribute, 0);
		if (tgtAttribute != -1)glEnableVertexAttribArray(tgtAttribute);// glVertexAttribDivisor(tgtAttribute, 0);
		if (uvAttribute != -1)glEnableVertexAttribArray(uvAttribute); //glVertexAttribDivisor(uvAttribute, 0);
		if (uvBlendAttribute != -1)glEnableVertexAttribArray(uvBlendAttribute);// glVertexAttribDivisor(uvBlendAttribute, 0);
	}
	void SendMatricesInstanced(GLuint MVP_vb_Id, GLuint M_vb_Id)
	{
		glUseProgram(programId);
		glBindBuffer(GL_ARRAY_BUFFER, MVP_vb_Id);
		if (transfMatrixInstanceAttribute != -1)
			for (int i = 0; i < 4; i++)
			{
				glVertexAttribPointer(transfMatrixInstanceAttribute + i, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(GLfloat), (void*)(i * 4 * sizeof(GLfloat)));
				glEnableVertexAttribArray(transfMatrixInstanceAttribute + i);
				glVertexAttribDivisor(transfMatrixInstanceAttribute + i, 1);
			}

		glBindBuffer(GL_ARRAY_BUFFER, M_vb_Id);
		if (modelMatrixInstanceAttribute != -1)
			for (int i = 0; i < 4; i++)
			{
				glVertexAttribPointer(modelMatrixInstanceAttribute + i, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(GLfloat), (void*)(i * 4 * sizeof(GLfloat)));
				glEnableVertexAttribArray(modelMatrixInstanceAttribute + i);
				glVertexAttribDivisor(modelMatrixInstanceAttribute + i, 1);
			}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}
	void SendMVP(float * const &MVP)
	{
		if (transfMatrixUniform != -1)
			glUniformMatrix4fv(transfMatrixUniform, 1, GL_FALSE, MVP);
	}
	void SendModelMatrix(float * const &modelMatrix)
	{
		if (modelMatrixUniform != -1)
			glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	}
	void SendCameraPos(float * const &cameraPos)
	{
		if (cameraPosUniform != -1)
			glUniform3f(cameraPosUniform, cameraPos[0], cameraPos[1], cameraPos[2]);
	}
	void SendObjectColor(float * const &objectColor)
	{
		if (objectColorUniform != -1)
			glUniform3f(objectColorUniform, objectColor[0], objectColor[1], objectColor[2]);
	}
	void SendLightColor(float * const &lightColor)
	{
		if (lightColorUniform != -1)
			glUniform3f(lightColorUniform, lightColor[0], lightColor[1], lightColor[2]);
	}
	void SendLightPos(float * const &lightPos)
	{
		if (lightPosUniform != -1)
			glUniform3f(lightPosUniform, lightPos[0], lightPos[1], lightPos[2]);
	}
	void SendViewPos(float * const &viewPos)
	{
		if (viewPosUniform != -1)
			glUniform3f(viewPosUniform, viewPos[0], viewPos[1], viewPos[2]);
	}


	~Shader() {
		glDeleteProgram(programId);
		glDeleteShader(vertexShaderId);
		glDeleteShader(fragmentShaderId);
	}
};

class ResourceManager
{
private:
	static ResourceManager* uniqueRManager;
	ResourceManager() { printf("new ResourceManager\n"); };
	unordered_map<int, Model> models;
	unordered_map<int, Texture> textures;
	unordered_map<int, Shader> shaders;
	unordered_map<int, ModelResource> modelResources;
	unordered_map<int, TextureResource> textureResources;
	unordered_map<int, ShaderResource> shaderResources;
	void InitModelResources(xml_node<> * const &pRoot);
	void InitShaderResources(xml_node<> * const &pRoot);
	void InitTextureResources(xml_node<> * const &pRoot);
public:
	void Init();
	static ResourceManager* getInstance();
	Model* reqModel(int const &id_model);
	Texture* reqTexture(int const &id_texture);
	Shader* reqShader(int const &id_shader);
	~ResourceManager() { uniqueRManager = NULL; printf("delete ResourceManager\n");
	};



};
