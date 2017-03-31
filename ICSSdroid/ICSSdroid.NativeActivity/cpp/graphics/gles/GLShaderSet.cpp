/*
(c) 2016,2017 Grain

This file is part of ICSEdit.

ICSEdit is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

ICSEdit is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with ICSEdit.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "graphics/gles/GLShaderSet.h"

using ICSS::graphics::gles::GLShaderSet;

static char GLShaderSet_loc_shaderName[2][19] = {
	"GL_FRAGMENT_SHADER",
	"GL_VERTEX_SHADER",
};

#define ShaderName(x) (std::string((x) >= GL_FRAGMENT_SHADER && (x) <= GL_VERTEX_SHADER ? GLShaderSet_loc_shaderName[((x) - GL_FRAGMENT_SHADER)] : "unkown"))

template<typename FunGetIv, typename FunGetLog>
std::string getGLLogStr(GLuint obj, FunGetIv funGetIv, FunGetLog funGetLog)
{
	GLint len = -1;
	funGetIv(obj, GL_INFO_LOG_LENGTH, &len);
	if (len > 1) {
		std::unique_ptr<char[]> infoLog(new char[len]);
		funGetLog(obj, len, nullptr, infoLog.get());
		return std::string(infoLog.get());
	}
	else {
		return std::string();
	}
		
}
std::string getGLShaderLogInfo(GLuint obj)
{
	return getGLLogStr(obj, glGetShaderiv, glGetShaderInfoLog);
}
std::string getGLProgramLogInfo(GLuint obj)
{
	return getGLLogStr(obj, glGetProgramiv, glGetProgramInfoLog);
}

GLShaderSet::GLShaderSet(void)
	: m_program(0),
	m_vShader(0),
	m_fShader(0)
{
}

GLShaderSet GLShaderSet::createFromString(const std::string &vShader, const std::string &fShader, const std::map<GLuint, std::string> &attribs)
{
	GLShaderSet shader;

	//Create vertex shader
	shader.m_vShader = shader.createShader(GL_VERTEX_SHADER, vShader);

	//Create fragment shader
	shader.m_fShader = shader.createShader(GL_FRAGMENT_SHADER, fShader);

	//create program and attach shaders
	shader.m_program = shader.createProgram();
	glAttachShader(shader.m_program, shader.m_vShader);
	glAttachShader(shader.m_program, shader.m_fShader);

	//set vshader attribs
	
	for(auto it : attribs)
	{
		glBindAttribLocation(shader.m_program, it.first, it.second.c_str());
	}

	//link shaders
	shader.linkProgram();

	return shader;
}

GLShaderSet::~GLShaderSet(void)
{
	if(m_program)
	{
		glDeleteProgram(m_program);
		m_program = 0;
	}
	if(m_fShader)
	{
		glDeleteShader(m_fShader);
		m_fShader = 0;
	}
	if(m_vShader)
	{
		glDeleteShader(m_vShader);
		m_vShader = 0;
	}
}

GLShaderSet::GLShaderSet(GLShaderSet && ary)
{
	this->m_vShader = ary.m_vShader;
	this->m_fShader = ary.m_fShader;
	this->m_program = ary.m_program;
	ary.m_vShader = 0;
	ary.m_fShader = 0;
	ary.m_program = 0;
}

GLShaderSet & GLShaderSet::operator=(GLShaderSet && ary)
{
	this->m_vShader = ary.m_vShader;
	this->m_fShader = ary.m_fShader;
	this->m_program = ary.m_program;
	ary.m_vShader = 0;
	ary.m_fShader = 0;
	ary.m_program = 0;

	return *this;
}

/*
GLint ICSS::graphics::gles::GLShaderSet::getAttribLocation(const std::string & name)
{
	return glGetAttribLocation(m_program, name.c_str());
}

GLint ICSS::graphics::gles::GLShaderSet::getUniformLocation(const std::string & name)
{
	return glGetUniformLocation(m_program, name.c_str());
}
*/

GLuint GLShaderSet::createShader(GLenum type, const std::string & src)
{
	GLuint shader;
	GLint success;

	shader = glCreateShader(type);
	if (shader == 0)
		throw std::runtime_error("glCreateShader(" + ShaderName(type) + ") failed");

	const char *sSrc = src.c_str();
	glShaderSource(shader, 1, &sSrc, 0);

	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		std::string errmsg = getGLShaderLogInfo(shader);
		glDeleteShader(shader);
		throw std::runtime_error("glCompileShader(" + ShaderName(type) + ") failed.\n Error:" + errmsg);
	}
	return shader;
}

GLuint GLShaderSet::createProgram(void)
{
	GLuint program;
	program = glCreateProgram();
	if (program == 0)
		throw std::runtime_error("glCreateProgram() failed.");

	return program;
}

void GLShaderSet::linkProgram(void)
{
	GLint success;
	glLinkProgram(m_program);
	glGetProgramiv(m_program, GL_LINK_STATUS, &success);
	if (!success) {
		throw std::runtime_error("glLinkProgram() failed. Error:" + getGLProgramLogInfo(m_program));
	}
}
