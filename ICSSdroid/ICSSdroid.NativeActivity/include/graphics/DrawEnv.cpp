#include "pch.h"
#include "DrawEnv.h"

using namespace ICSS::graphics;

DrawEnv::DrawEnv(void)
	: m_texture(0),
	m_program(0)
{
}

void ICSS::graphics::DrawEnv::setShader(const ICSS::graphics::gles::GLShaderSet & shader)
{
	if(m_program != shader.program())
		glUseProgram(shader.program());
	m_program = shader.program();
}
