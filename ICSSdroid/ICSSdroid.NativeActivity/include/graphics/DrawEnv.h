#pragma once
#include "graphics/gles/GLShaderSet.h"

namespace ICSS{
namespace graphics{

	class DrawEnv {
		int m_width;
		int m_height;

		GLuint m_texture;
		GLuint m_program;
	public:
		DrawEnv(void);

		int width(void) const { return m_width; }
		int height(void) const { return m_height; }
		void setWidth(int w) { m_width = w; }
		void setHeight(int h) { m_height = h; }

		void setTexture(GLuint texture);
		void setShader(const ICSS::graphics::gles::GLShaderSet &shader);
	private:
		//DrawEnv(void);
	};

}
}