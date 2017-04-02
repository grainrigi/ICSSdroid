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

#include "graphics/MemCanvasRenderTexturePool.h"
#include "graphics/MemTexturedCanvasRGBA8.h"

using namespace ICSS::graphics;

ICSS::graphics::MemCanvasRenderTexturePool::TextureUnit::TextureUnit(void)
	: m_id(ObtainNewID()),
	m_texture(true)
{
	memset(m_map, 0, sizeof(uint8_t) * BLOCK_HORZ_COUNT * BLOCK_VERT_COUNT);
	m_texture.bind();
	//uint32_t *mem = new uint32_t[textureHeight() * textureWidth()];
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, BLOCK_WIDTH * BLOCK_HORZ_COUNT, BLOCK_HEIGHT * BLOCK_VERT_COUNT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	//delete mem;
}

ICSS::graphics::MemCanvasRenderTexturePool::TextureUnit::TextureUnit(uint32_t obtained_id)
	: m_texture(true),
	m_id(obtained_id)
{
	memset(m_map, 0, sizeof(uint8_t) * BLOCK_HORZ_COUNT * BLOCK_VERT_COUNT);
	m_texture.bind();
	//uint32_t *mem = new uint32_t[textureHeight() * textureWidth()];
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, BLOCK_WIDTH * BLOCK_HORZ_COUNT, BLOCK_HEIGHT * BLOCK_VERT_COUNT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	//delete mem;
}

MemCanvasRenderTexturePool::SpaceUnit ICSS::graphics::MemCanvasRenderTexturePool::TextureUnit::ObtainNewSpace(int width, int height)
{
	SpaceUnit space{ 0, 0, 0, 0, 0 };

	int sw = width / BLOCK_WIDTH + (width % BLOCK_WIDTH ? 1 : 0), sh = height / BLOCK_HEIGHT + (height % BLOCK_HEIGHT ? 1 : 0);

	int cx = 0;
	int cy = 0;

	bool found = false;
	int x = 0, y = 0;
	uint8_t *map = m_map;

	while (true)
	{
		if (BLOCK_VERT_COUNT - cy < sh)
			break;
		while (true)
		{
			if (BLOCK_HORZ_COUNT - cx < sw)
				break;
			if (map[cx + cy * BLOCK_HORZ_COUNT])
			{
				cx++;
				x = 0;
				continue;
			}
			else
			{
				found = true;
				for (int jy = 0; jy < sh; jy++)
				{
					if (map[x + cx + (jy + cy) * BLOCK_HORZ_COUNT])
					{
						found = false;
						break;
					}
				}
				if (found)
				{
					found = false;
					x++;
					if (x == sw)
					{
						found = true;
						break;
					}
				}
				else
				{
					cx++;
					x = 0;
					continue;
				}
			}
		}
		if (found)
			break;
		else
		{
			x = 0;
			y = 0;
			cy++;
			cx = 0;
		}
	}

	if (found)
	{
		for (y = cy; y < sh + cy; y++)
		{
			for (x = cx; x < sw + cx; x++)
				map[x + y * BLOCK_HORZ_COUNT] = 1;
		}
		space.id = ObtainNewID();
		space.x = cx;
		space.y = cy;
		space.w = sw;
		space.h = sh;
		m_spaces.insert(std::make_pair(space.id, space));
	}

	return space;
}

void ICSS::graphics::MemCanvasRenderTexturePool::TextureUnit::ReleaseSpace(int spaceid)
{
	uint8_t *map = m_map;
	auto it = m_spaces.find(spaceid);
	if (it != m_spaces.end())
	{
		SpaceUnit space = it->second;
		for (int y = space.y; y < space.y + space.h; y++)
		{
			for (int x = space.x; x < space.x + space.w; x++)
				map[x + y * BLOCK_HORZ_COUNT] = 0;
		}
		m_spaces.erase(it);
	}
}

void ICSS::graphics::MemCanvasRenderTexturePool::TextureUnit::UpdateTexture(int spaceid, const MemCanvasRGBA8 & canvas)
{
	auto space = m_spaces.find(spaceid);
	if(space != m_spaces.end())
	{
		glBindTexture(GL_TEXTURE_2D, m_texture.handle());
		glTexSubImage2D(GL_TEXTURE_2D, 0, space->second.x * BLOCK_WIDTH, space->second.y * BLOCK_HEIGHT, canvas.width(), canvas.height(), GL_RGBA, GL_UNSIGNED_BYTE, canvas.pixels());
	}
}

const MemCanvasRenderTexturePool::SpaceUnit & ICSS::graphics::MemCanvasRenderTexturePool::TextureUnit::GetSpaceInfo(int spaceid)
{
	return m_spaces[spaceid];
}

ICSS::graphics::MemCanvasRenderTexturePool::MemCanvasRenderTexturePool(void)
{
}

MemTexturedCanvasRGBA8 ICSS::graphics::MemCanvasRenderTexturePool::ObtainCanvas(int width, int height)
{
	MemTexturedCanvasRGBA8 canvas(width, height);
	SpaceUnit space;
	bool found = false;

	for (auto it = m_units.begin(); it != m_units.end(); it++)
	{
		space = (it->second)->ObtainNewSpace(width, height);

		//successfully obtained
		if(space.w != 0)
		{
			canvas.m_spaceid = space.id;
			canvas.m_txunit = it->second;
			canvas.m_pool = this;
			found = true;
			break;
		}
	}

	if(!found)
	{
		int id = ObtainNewID();
		m_units.insert(std::make_pair(id, std::shared_ptr<TextureUnit>(new TextureUnit(id))));
		space = (m_units[id])->ObtainNewSpace(width, height);
		canvas.m_spaceid = space.id;
		canvas.m_txunit = m_units[id];
		canvas.m_pool = this;
	}

	return canvas;
}

std::shared_ptr<MemCanvasRenderTexturePool::TextureUnit> ICSS::graphics::MemCanvasRenderTexturePool::GetTxUnit(uint32_t id)
{
	return m_units.find(id)->second;
}

int ICSS::graphics::MemCanvasRenderTexturePool::ObtainNewID(void)
{
	static std::atomic_uint_fast32_t ctr(0);

	return ctr++;
}
