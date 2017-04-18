/*
(c) 2016,2017 Grain

This file is part of ICSSdroid.

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

#include "graphics/gles/GLVBO.h"

using ICSS::graphics::gles::GLVBO;
using ICSS::graphics::gles::GLVBOTarget;

ICSS::graphics::gles::GLVBO::GLVBO(void)
	: m_buffer(0),
	m_target(0),
	m_size(0),
	m_usage(0),
	m_isMapping(false)
{
}

GLVBO::GLVBO(GLVBOTarget target, GLenum usage)
	: m_buffer(0),
	m_target((GLenum)target),
	m_size(0),
	m_usage(usage),
	m_isMapping(false)
{
	glGenBuffers(1, &m_buffer);
	if(m_buffer == 0)
		throw std::runtime_error("glGenBuffers() failed.");
}

GLVBO::GLVBO(GLVBOTarget target, GLsizeiptr size, GLenum usage)
	: m_buffer(0),
	m_target((GLenum)target),
	m_size(size),
	m_usage(usage),
	m_isMapping(false)
{
	glGenBuffers(1, &m_buffer);
	if (m_buffer == 0)
		throw std::runtime_error("glGenBuffers() failed.");

	this->extend(size);
}

GLVBO::~GLVBO(void)
{
	if(m_buffer != 0)
		glDeleteBuffers(1, &m_buffer);
}

GLVBO::GLVBO(GLVBO && vbo)
	: m_buffer(vbo.m_buffer),
	m_target(vbo.m_target),
	m_size(vbo.m_size),
	m_usage(vbo.m_usage),
	m_isMapping(vbo.m_isMapping)
{
	vbo.m_buffer = 0;
}

GLVBO & GLVBO::operator=(GLVBO && vbo)
{
	this->m_buffer = vbo.m_buffer;
	this->m_target = vbo.m_target;
	this->m_size = vbo.m_size;
	this->m_isMapping = vbo.m_isMapping;
	this->m_usage = vbo.m_usage;
	vbo.m_target = 0;

	return *this;
}

void GLVBO::uploadData(GLsizeiptr size, const GLvoid * data)
{
	if (size > this->m_size)
	{
		glBindBuffer(m_target, m_buffer);
		glBufferData(m_target, size, data, m_usage);
		this->m_size = size;
		glBindBuffer(m_target, 0);
	}
	else {
		glBindBuffer(m_target, m_buffer);
		glBufferSubData(m_target, 0, size, data);
		glBindBuffer(m_target, 0);
	}
}

void GLVBO::uploadDataRange(GLsizeiptr size, const GLvoid * data, GLuint offset)
{
	extend(size + offset, true);

	glBindBuffer(m_target, m_buffer);
	glBufferSubData(m_target, offset, size, data);
	glBindBuffer(m_target, 0);
}

void GLVBO::extend(GLsizeiptr size, bool preserve)
{
	if(size <= this->m_size)
		return;

	/*
	uint8_t *buf = nullptr;
	if(preserve)
	{
		buf = new uint8_t[this->m_size];
		uint8_t *prev = reinterpret_cast<uint8_t*>(this->map(GL_MAP_READ_BIT));
		memcpy(buf, prev, this->m_size);
		this->unmap();
	}
	*/
	glBindBuffer(m_target, m_buffer);
	glBufferData(m_target, size, NULL, m_usage);
	/*
	if(preserve)
	{
		uint8_t *cur = reinterpret_cast<uint8_t*>(this->map(GL_MAP_WRITE_BIT));
		memcpy(cur, buf, this->m_size);
		this->unmap();
		delete buf;
	}
	*/
	glBindBuffer(m_target, 0);

	this->m_size = size;
}

/*
void * ICSS::graphics::gles::GLVBO::map(GLbitfield access)
{
	if(m_isMapping)
		return nullptr;
	else
	{
		m_isMapping = true;
		glBindBuffer(m_target, m_buffer);
		return glMapBufferRange(m_target, 0, this->m_size, access);
	}
}

void * GLVBO::map(GLintptr offset, GLsizeiptr size, GLbitfield access)
{
	if (m_isMapping)
		return nullptr;
	else if((offset + size) > m_size)
		throw std::out_of_range("specified offset or size is out of range.");
	else
	{
		m_isMapping = true;
		glBindBuffer(m_target, m_buffer);
		return glMapBufferRange(m_target, offset, size, access);
	}
}

void GLVBO::unmap(void)
{
	glBindBuffer(m_target, m_buffer);
	glUnmapBuffer(m_target);
	this->m_isMapping = false;
}
*/

void GLVBO::bind(void)
{
	/*
	if(m_isMapping)
		this->unmap();
		*/
	glGetError();
	glBindBuffer(m_target, m_buffer);
	GLenum err = glGetError();
	if (err != 0)
		throw std::runtime_error("glBindBuffer failed.");
}


