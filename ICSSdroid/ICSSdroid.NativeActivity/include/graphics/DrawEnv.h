#pragma once
#include "pch.h"
#include "graphics/GraphicsManager.h"
#include "graphics/gles/GLShaderSet.h"

namespace ICSS{
namespace graphics{

	class DrawEnv {
		friend ICSS::graphics::GraphicsManager;

		GLuint m_texture;
		GLuint m_program;
	public:
		DrawEnv(void);

		void setTexture(GLuint texture);
		void setShader(const ICSS::graphics::gles::GLShaderSet &shader);
	private:
		//DrawEnv(void);
	};

}
}