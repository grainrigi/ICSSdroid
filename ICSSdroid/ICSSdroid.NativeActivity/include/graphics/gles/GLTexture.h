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

#pragma once
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
		bool uploadImage(const char *pixels, int width, int height, GLint internalFormat, GLenum format, GLenum type);

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

