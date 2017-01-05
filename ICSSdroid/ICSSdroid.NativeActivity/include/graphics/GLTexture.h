#pragma once
#include "pch.h"
#include "SLES/OpenSLES.h"

namespace ICSS{
namespace graphics{
namespace gles{

	class GLTexture {
		GLuint m_texture;

	public:
		GLTexture(void);

		GLuint handle(void) const { return m_texture; }
		bool operator==(const GLTexture &tex) { return m_texture == tex.m_texture; };
	private:

	};

}
}
}

