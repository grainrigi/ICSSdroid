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

#include "graphics/gles/GLTexture.h"

using ICSS::graphics::gles::GLTexture;
using ICSS::file::ImageFile;

ICSS::graphics::gles::GLTexture::GLTexture(bool init)
	: m_texture(0)
{
	if(init)
		this->init();
}

ICSS::graphics::gles::GLTexture::~GLTexture(void)
{
	if(m_texture != 0)
	{
		glDeleteTextures(1, &m_texture);
		m_texture = 0;
	}
}

void GLTexture::init(void)
{
	if(m_texture)
	{
		glDeleteTextures(1, &m_texture);
		m_texture = 0;
	}

	glGenTextures(1, &m_texture);

	glBindTexture(GL_TEXTURE_2D, m_texture);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	this->setMagFilter(GL_NEAREST);
	this->setMinFilter(GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);
}

bool ICSS::graphics::gles::GLTexture::uploadImage(const file::ImageFile & img)
{
	/*constexpr GLenum glChannelFormat[2]
	{
		GL_RGB,
		GL_RGBA,
	};*/

	this->bind();

	if(img.getChannelCount() <= 2 || img.getChannelCount() > 5)
		return false;

    assert(img.getChannelCount() - 3 ==0 || img.getChannelCount() - 3 == 1);
	glGetError();
	bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getX(), img.getY(), 0, GL_RGBA/*glChannelFormat[img.getChannelCount() - 3]*/, GL_UNSIGNED_BYTE, img.getPixels());
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
		throw std::runtime_error("glTexImage2D failed.Error:%d" + std::to_string(err) + FILE_FUNC_SIG);

	return true;
}

bool ICSS::graphics::gles::GLTexture::uploadImage(const file::ImageFile & img, GLint internalFormat, GLenum format, GLenum type)
{
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, img.getX(), img.getY(), 0, format, type, img.getPixels());
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
		throw std::runtime_error("glTexImage2D failed.Error:%d" + std::to_string(err) + FILE_FUNC_SIG);

	return true;
}

bool ICSS::graphics::gles::GLTexture::uploadImage(const char * pixels, int width, int height, GLint internalFormat, GLenum format, GLenum type)
{
	glGetError();
	bind();
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, pixels);
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
		throw std::runtime_error("glTexImage2D failed.Error:%d" + std::to_string(err) + FILE_FUNC_SIG);

	return true;
}

void GLTexture::bind(void) const
{
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

void GLTexture::bind(GLuint target) const
{
	glBindTexture(target, m_texture);
}
