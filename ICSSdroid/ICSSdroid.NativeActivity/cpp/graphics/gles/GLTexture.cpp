#include "pch.h"
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
}

bool ICSS::graphics::gles::GLTexture::uploadImage(const file::ImageFile & img)
{
	constexpr GLenum glChannelFormat[2]
	{
		GL_RGB,
		GL_RGBA,
	};

	if(img.getChannelCount() <= 2 || img.getChannelCount() > 5)
		return false;

    assert(img.getChannelCount() - 3 ==0 || img.getChannelCount() - 3 == 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getX(), img.getY(), 0, glChannelFormat[img.getChannelCount() - 3], GL_UNSIGNED_BYTE, img.getPixels());
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

void GLTexture::bind(void) const
{
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

void GLTexture::bind(GLuint target) const
{
	glBindTexture(target, m_texture);
}
