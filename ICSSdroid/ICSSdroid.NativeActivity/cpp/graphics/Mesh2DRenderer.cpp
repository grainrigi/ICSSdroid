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

#include "graphics/Mesh2DRenderer.h"

using namespace ICSS::graphics;
using namespace ICSS::graphics::gles;

Mesh2DRenderer::~Mesh2DRenderer(void)
{

}

void ICSS::graphics::Mesh2DRenderer::draw(DrawEnv * env, Mesh2D & mesh)
{
	if(mesh.positionOffset() == -1)
		return;
	env->setShader(m_shader_tex);

	mesh.bindVertBuf();
	
	
	//use texture (requires that texture is bound to unit-0 in advance)
	if(mesh.coordOffset() != -1) {
		env->setShader(m_shader_tex);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)mesh.positionOffset());
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)mesh.coordOffset());
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
	}
	//use color
	else if(mesh.colorOffset() != -1) {
		env->setShader(m_shader_vc);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(mesh.positionOffset()));
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)mesh.colorOffset());
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
	}
	//use neither
	else
	{
		env->setShader(m_shader_vc);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)mesh.positionOffset());
		glEnableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}
	

	//draw
	if(mesh.indexOffset() != -1) {
		glDrawElements(GL_TRIANGLES, mesh.elemCount(), GL_UNSIGNED_SHORT, (GLvoid*)mesh.indexOffset());
		mesh.bindIdxBuf();
	}
	else
		glDrawArrays(GL_TRIANGLE_STRIP, 0, mesh.vertCount());
}

void ICSS::graphics::Mesh2DRenderer::draw(DrawEnv * env, Mesh2D & mesh, const gles::GLShaderSet & shader, const Mesh2D::ShaderAttributes & attr)
{
	env->setShader(shader);
	mesh.bindVertBuf();

	//Set attributes
	if(attr.attr_pos >= 0)
	{
		if (mesh.positionOffset() != -1)
		{
			glVertexAttribPointer(attr.attr_pos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)mesh.positionOffset());
			glEnableVertexAttribArray(attr.attr_pos);
		}
		else
			glDisableVertexAttribArray(attr.attr_pos);
	}
	if (attr.attr_uv >= 0)
	{
		if (mesh.coordOffset() != -1)
		{
			glVertexAttribPointer(attr.attr_uv, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)mesh.coordOffset());
			glEnableVertexAttribArray(attr.attr_uv);
		}
		else
			glDisableVertexAttribArray(attr.attr_uv);
	}
	if (attr.attr_color >= 0)
	{
		if (mesh.colorOffset() != -1)
		{
			glVertexAttribPointer(attr.attr_color, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)mesh.colorOffset());
			glEnableVertexAttribArray(attr.attr_color);
		}
		else
			glDisableVertexAttribArray(attr.attr_color);
	}

	//draw
	if (mesh.indexOffset() != -1) {
		glDrawElements(GL_TRIANGLES, mesh.elemCount(), GL_UNSIGNED_SHORT, (GLvoid*)mesh.indexOffset());
		mesh.bindIdxBuf();
	}
	else
		glDrawArrays(GL_TRIANGLE_STRIP, 0, mesh.vertCount());
}

ICSS::graphics::Mesh2DRenderer::Mesh2DRenderer(bool init)
{
	if(init)
		this->initShader();
}

void ICSS::graphics::Mesh2DRenderer::LateInitialize(void)
{
	initShader();
}

void ICSS::graphics::Mesh2DRenderer::initShader(void)
{
	std::string vShader_tex{ 
		//"#version 120\n"
		"attribute mediump vec4 attr_pos; "
		"attribute mediump vec2 attr_uv;"
		"varying mediump vec2 vary_uv;"
		"void main(){"
		"  gl_Position = attr_pos;"
		"  vary_uv = attr_uv;"
		"}" };
	std::string fShader_tex{
		//"#version 120\n"
		"uniform sampler2D unif_texture;"
		"varying mediump vec2 vary_uv;"
		"void main(){"
		"  gl_FragColor = texture2D(unif_texture, vary_uv);"
		"}" };

	std::string vShader_vc{
		//"#version 120\n"
		"attribute mediump vec4 attr_pos; "
		"attribute mediump vec4 attr_color;"
		"varying mediump vec4 vary_color;"
		"void main(){"
		"  gl_Position = attr_pos;"
		"  vary_color = attr_color;"
		"}" };
	std::string fShader_vc{
		//"#version 120\n"
		"varying mediump vec4 vary_color;"
		"void main(){"
		"  gl_FragColor = vary_color;"
		"}" };

	m_shader_tex = GLShaderSet::createFromString(
		vShader_tex, 
		fShader_tex, 
		{
			{ 0, "attr_pos" },
			{ 1, "attr_uv" }
		});
	glUniform1i(glGetUniformLocation(m_shader_tex.program(), "unif_texture"), 0);

	m_shader_vc = GLShaderSet::createFromString(
		vShader_vc,
		fShader_vc,
		{
			{ 0, "attr_pos" },
			{ 1, "attr_color" }
		});
}
