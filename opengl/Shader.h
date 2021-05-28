#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class Shader
{
public:
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);// ��������ȡ��������ɫ��
	void use();// ʹ��/�������

	// uniform���ߺ���
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;

	GLuint getProgramID() { return m_uiID; }

private:
	void checkCompileErrors(unsigned int shader, std::string type);

private:
	GLuint m_uiID;
};

#endif