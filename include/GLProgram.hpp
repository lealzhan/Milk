//http://learnopengl-cn.readthedocs.io/zh/latest/01%20Getting%20started/05%20Shaders/

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>; // ����glew����ȡ���еı���OpenGLͷ�ļ�

#include "basic_struct.h"

class Program
{
public:
	// ����ID
	GLuint ID;
	// ��������ȡ��������ɫ��
	Program(const GLchar* vShaderCode, const GLchar* fShaderCode, int flag);
	Program(const GLchar* vertexPath, const GLchar* fragmentPath);
	// ʹ�ó���
	void Use();
	void UnUse();
	bool m_bInited;

	void setUniform(const std::string& uniformName, int x);
	void setUniform(const std::string& uniformName, float x);
	void setUniform(const std::string& uniformName, double x);
	void setUniform(const std::string& uniformName, float x, float y);
	void setUniform(const std::string& uniformName, float x, float y, float z);
	void setUniform(const std::string& uniformName, int x, int y, int z, int w);
	void setUniform(const std::string& uniformName, int* v, int length);
	void setTexture(const std::string& uniformName, int texID, GLint texture);
};

#endif