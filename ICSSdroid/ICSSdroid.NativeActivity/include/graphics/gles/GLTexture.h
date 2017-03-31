#pragma once
#include "pch.h"
#include "util/file/ImageFile.h"

namespace ICSS{
namespace graphics{
namespace gles{

	class GLTexture {
		GLuint m_texture;

	public:
		explicit GLTexture(bool init = false);
		~GLTexture(void);

		GLTexture(const GLTexture &img) = delete;
		GLTexture &operator=(const GLTexture &img) = delete;
		GLTexture(GLTexture &&img) = default;
		GLTexture &operator=(GLTexture &&img) = default;

		void init(void);

		bool uploadImage(const file::ImageFile &img);
		bool uploadImage(const file::ImageFile &img, GLint internalFormat, GLenum format, GLenum type);

		GLuint handle(void) const { return m_texture; }
		void bind(void) const;
		void bind(GLuint target) const;
		bool operator==(const GLTexture &tex) { return m_texture && m_texture == tex.m_texture; }

		void setMagFilter(GLenum filter) { bind(); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter); }
		void setMinFilter(GLenum filter) { bind(); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter); }
	private:

	};

}
}
}

