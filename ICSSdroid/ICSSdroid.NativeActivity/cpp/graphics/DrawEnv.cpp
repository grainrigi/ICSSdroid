/*
(c) 2016,2017 Grain

This file is part of ICSSdroid.

ICSSdroid is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

ICSSdroid is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with ICSSdroid.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "graphics/DrawEnv.h"

using namespace ICSS::graphics;

DrawEnv::DrawEnv(void)
	: m_texture(0),
	m_program(0)
{
}

void ICSS::graphics::DrawEnv::setTexture(GLuint texture)
{
	if (m_texture != texture)
		glBindTexture(GL_TEXTURE_2D, texture);
}

void ICSS::graphics::DrawEnv::setShader(const ICSS::graphics::gles::GLShaderSet & shader)
{
		glUseProgram(shader.program());
	m_program = shader.program();
}
