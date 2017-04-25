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

#include "graphics/MemCanvasRenderer.h"
#include "graphics/MemTexturedCanvasRGBA8.h"

using namespace ICSS::graphics;
using namespace ICSS::graphics::gles;

ICSS::graphics::MemCanvasRenderer::MemCanvasRenderer(void)
	: m_renderer{nullptr}
{
}

ICSS::graphics::MemCanvasRenderer::MemCanvasRenderer(Mesh2DRenderer *renderer)
	: m_renderer{renderer}
{
	/*m_texture.init();
	m_texture.uploadImage(nullptr, 2048, 2048, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);*/
	
	
	init();
}

void ICSS::graphics::MemCanvasRenderer::Render(DrawEnv * env, const MemCanvasRGBA8 & canvas, const wnd::BoundingBox & bb)
{
	/*env->setTexture(m_texture.handle());
	glTexSubImage2D(GL_TEXTURE_2D, 0, GL_RGBA, canvas.width(), canvas.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, canvas.pixels());

	float rw = (float)bb.GetWidth() / (float)env->width() * 2.0f;
	float rh = (float)bb.GetHeight() / (float)env->height() * 2.0f;
	float rx = (float)bb.GetX() / (float)env->width() * 2.0f - 1.0f;
	float ry = 1.0f - (float)bb.GetY() / (float)env->height() * 2.0f;

	//float tw = (float)canvas.width() / 2048.0f;
	//float th = (float)canvas.height() / 2048.0f;
	float tw = 1.0f, th = 1.0f;

	Position *pos_ptr = &(*m_mesh.positions())[0];
	pos_ptr[0].x = rx; pos_ptr[0].y = ry;
	pos_ptr[1].x = rx + rw; pos_ptr[1].y = ry;
	pos_ptr[2].x = rx; pos_ptr[2].y = ry - rh;
	pos_ptr[3].x = rx + rw; pos_ptr[3].y = ry - rh;
	Coordinate *coord_ptr = &(*m_mesh.coordinates())[0];
	coord_ptr[1].x = tw;
	coord_ptr[2].y = th;
	coord_ptr[3].x = tw; coord_ptr[3].y = th;

	m_mesh.upload();
	m_renderer->draw(env, m_mesh);*/
}

void ICSS::graphics::MemCanvasRenderer::RegisterElement(MemTexturedCanvasRGBA8 & canvas, const wnd::BoundingBox & bbox, bool autoclip)
{
	auto txunit = canvas.m_txunit.lock();
	auto it = m_dunits.find(txunit->id());

	if(it == m_dunits.end())
	{
		it = m_dunits.insert(std::make_pair(txunit->id(), DrawUnit(txunit))).first;
	}

	if(!autoclip)
		it->second.RegisterElement(
			canvas.m_spaceid,
			canvas.width(),
			canvas.height(),
			bbox
		);
	else
		it->second.RegisterElement(
			canvas.m_spaceid,
			bbox.GetWidth(),
			bbox.GetHeight(),
			bbox
		);
}

void ICSS::graphics::MemCanvasRenderer::RenderAll(DrawEnv *env)
{
	for (auto &dunit : m_dunits)
		dunit.second.DrawRegistered(env, *this);
}

void ICSS::graphics::MemCanvasRenderer::initShader(void)
{
	std::string vShader{
		"uniform mat4 unif_disp;"
		"attribute mediump vec4 attr_pos; "
		"attribute mediump vec2 attr_uv;"
		"varying mediump vec2 vary_uv;"
		"void main(){"
		"  gl_Position = attr_pos * unif_disp + vec4(-1.0, 1.0, 0.0, 0.0);"
		"  vary_uv = attr_uv;"
		"}"
	};

	std::string fShader{
		"uniform sampler2D unif_texture;"
		"varying mediump vec2 vary_uv;"
		"void main(){"
		"  gl_FragColor = clamp(texture2D(unif_texture, vary_uv), 0.0, 1.0);"
		"}"
	};

	m_shader = GLShaderSet::createFromString(
		vShader,
		fShader,
		{
			{ 0, "attr_pos" },
			{ 1, "attr_uv" }
		}
	);

	glUniform1i(m_shader.getUniformLocation("unif_texture"), 0);
	m_unif_mat_loc = m_shader.getUniformLocation("unif_disp");
}

void ICSS::graphics::MemCanvasRenderer::LateInitialize(Mesh2DRenderer * renderer)
{
	m_renderer = renderer;
	init();
}

void ICSS::graphics::MemCanvasRenderer::init(void)
{
	m_mesh = Mesh2D(4, Mesh2D::ATTR_POSITION | Mesh2D::ATTR_COORD, GL_DYNAMIC_DRAW);
	memset(&(*m_mesh.positions())[0], 0, sizeof(Position) * 4);
	memset(&(*m_mesh.coordinates())[0], 0, sizeof(Coordinate) * 4);

	initShader();
}

ICSS::graphics::MemCanvasRenderer::DrawUnit::DrawUnit(std::weak_ptr<MemCanvasRenderTexturePool::TextureUnit> txunit)
	: m_txunit(txunit),
	m_mesh(Mesh2D::ATTR_POSITION | Mesh2D::ATTR_COORD, GL_DYNAMIC_DRAW)
{
}

void ICSS::graphics::MemCanvasRenderer::DrawUnit::RegisterElement(int spaceid, int texwidth, int texheight, const wnd::BoundingBox & bbox)
{
	int vertPos = m_mesh.vertCount();

	m_mesh.setVertCount(m_mesh.vertCount() + 6);

	float rw = (float)bbox.GetWidth();
	float rh = (float)bbox.GetHeight();
	float rx = (float)bbox.GetX();
	float ry = (float)bbox.GetY();
	//float rw = 1.0f, rh = 1.0f, rx = -1.0f, ry = 1.0f;

	float tw = (float)texwidth / (float)MemCanvasRenderTexturePool::textureWidth();
	float th = (float)texheight / (float)MemCanvasRenderTexturePool::textureHeight();
	auto &space = m_txunit.lock()->GetSpaceInfo(spaceid);
	float tx = space.x * (float)MemCanvasRenderTexturePool::BLOCK_WIDTH / (float)MemCanvasRenderTexturePool::textureWidth();
	float ty = space.y * (float)MemCanvasRenderTexturePool::BLOCK_HEIGHT / (float)MemCanvasRenderTexturePool::textureHeight();

	Position *pos_ptr = &(*m_mesh.positions())[vertPos];
	pos_ptr[0].x = rx; pos_ptr[0].y = ry;
	pos_ptr[1].x = rx; pos_ptr[1].y = ry;
	pos_ptr[2].x = rx + rw; pos_ptr[2].y = ry;
	pos_ptr[3].x = rx; pos_ptr[3].y = ry + rh;
	pos_ptr[4].x = rx + rw; pos_ptr[4].y = ry + rh;
	pos_ptr[5].x = rx + rw; pos_ptr[5].y = ry + rh;
	Coordinate *coord_ptr = &(*m_mesh.coordinates())[vertPos];
	coord_ptr[0].x = tx; coord_ptr[0].y = ty;
	coord_ptr[1].x = tx; coord_ptr[1].y = ty;
	coord_ptr[2].x = tx + tw; coord_ptr[2].y = ty;
	coord_ptr[3].x = tx; coord_ptr[3].y = ty + th;
	coord_ptr[4].x = tx + tw; coord_ptr[4].y = ty + th;
}

void ICSS::graphics::MemCanvasRenderer::DrawUnit::DrawRegistered(DrawEnv *env, const MemCanvasRenderer &parent)
{
	Matrix4f mat;

	auto txunit = m_txunit.lock();
	if(m_mesh.vertCount() > 0)
	{
		env->setTexture(txunit->texhandle());
		glBindTexture(GL_TEXTURE_2D, txunit->texhandle());
		env->setShader(parent.m_shader);
		
		Matrix4f mat;
		mat.identity();
		mat *= Matrix4f::createScale(2.0f / (float)env->width(), 2.0f / -(float)env->height(), 1.0f);
		//mat *= Matrix4f::createTranslation(0.0f, 0.0f, 0.0f);

		glUniformMatrix4fv(parent.m_unif_mat_loc, 1, GL_FALSE, mat.data);

		m_mesh.upload();
		parent.m_renderer->draw(env, m_mesh, parent.m_shader, { 0, 1, -1 });
		//parent.m_renderer->draw(env, m_mesh);

	}

	m_mesh.setVertCount(0);
}
