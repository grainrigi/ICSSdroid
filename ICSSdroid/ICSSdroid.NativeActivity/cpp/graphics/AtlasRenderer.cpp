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

#include "graphics/AtlasRenderer.h"

using namespace ICSS::graphics;
using namespace ICSS::graphics::gles;

ICSS::graphics::AtlasRenderer::AtlasRenderer(int count)
	: m_vbuf(gles::GLVBOTarget::ARRAY_BUFFER, GL_DYNAMIC_DRAW),
	m_ibuf(gles::GLVBOTarget::ELEMENT_ARRAY_BUFFER, GL_DYNAMIC_DRAW)
{
	m_capacity.resize(count);

	initShader();
}

void ICSS::graphics::AtlasRenderer::atlasCapacity(int capacity)
{
	m_vbuf.extend(requiredVBufSize(capacity));
	m_ibuf.extend(requiredIBufSize(capacity));

	m_atlasCapacity = capacity;
	m_Positions.resize(capacity * 4);
	m_Coordinates.resize(capacity * 4);
	m_Indices.resize(capacity * 4);
	m_Keys.reserve(capacity);
}

void ICSS::graphics::AtlasRenderer::updatePosition(int id, float x, float y, float w, float h)
{
	Position2D *ptr = static_cast<Position2D*>(m_Positions.buffer()) + id * 4;

	ptr[0].x = x - w / 2.0f;
	ptr[0].y = y + h / 2.0f;
	ptr[1].x = x - w / 2.0f;
	ptr[1].y = y - h / 2.0f;
	ptr[2].x = x + w / 2.0f;
	ptr[2].y = y + h / 2.0f;
	ptr[3].x = x + w / 2.0f;
	ptr[3].y = y - h / 2.0f;

	m_Positions.notifyRewrite(id * 4, id * 4 + 3);
}

void ICSS::graphics::AtlasRenderer::updateCoordinate(int id, float u_lt, float v_lt, float u_rb, float v_rb)
{
	Coordinate *ptr = static_cast<Coordinate*>(m_Coordinates.buffer()) + id * 4;

	ptr[0].x = u_lt;
	ptr[0].y = v_lt;
	ptr[1].x = u_lt;
	ptr[1].y = v_rb;
	ptr[2].x = u_rb;
	ptr[2].y = v_lt;
	ptr[3].x = u_rb;
	ptr[3].y = v_rb;

	m_Coordinates.notifyRewrite(id * 4, id * 4 + 3);
}

int ICSS::graphics::AtlasRenderer::addAtlas(float key)
{
	int id = this->m_capacity.obtainSpace();
	KeyBoundIndex idx { id, key };

	if(id != -1)
	{
		auto ins = std::upper_bound(m_Keys.begin(), m_Keys.end(), idx);
		m_IndicesUpdateStart = std::distance(m_Keys.begin(), ins);
		
		if (ins != m_Keys.end())
			m_Keys.insert(ins, idx);
		else
			m_Keys.push_back(idx);
	}

	return id;
}

void ICSS::graphics::AtlasRenderer::removeAtlas(int id)
{
	auto key = std::find_if(m_Keys.begin(), m_Keys.end(),
		[&](decltype(m_Keys)::value_type &val)
	{
		return val.id == id;
	});

	if(key != m_Keys.end())
	{
		m_IndicesUpdateStart = std::distance(m_Keys.begin(), key);
		m_capacity.freeSpace(id);
		m_Keys.erase(key);
	}
}

void ICSS::graphics::AtlasRenderer::drawAll(float width, float height)
{
	float vptrans[4]{ 2.0f / width, 2.0f / height, 1.0f, 1.0f };

	float vertices[]
	{
		-0.5f, 0.5f,
		-0.5f, -0.5f,
		0.5f, 0.5f,
		0.5f, -0.5f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f
	};

	uint16_t indices[]
	{
		0,1,2,1,3,2,
	};

	//m_vbuf.uploadData(sizeof(vertices), vertices);
	m_vbuf.bind();
	
	m_Positions.upload(GL_ARRAY_BUFFER, positionOffset());
	m_Coordinates.upload(GL_ARRAY_BUFFER, coordinateOffset());
	

	if (m_IndicesUpdateStart != -1)
	{
		this->updateIndices(m_IndicesUpdateStart);
		m_IndicesUpdateStart = -1;
	}

	m_ibuf.uploadData(sizeof(indices), indices);
	m_ibuf.bind();
	//m_Indices.upload(GL_ELEMENT_ARRAY_BUFFER, 0);

	//描画処理
	m_shader.use();
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)positionOffset());
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)coordinateOffset());

	glUniform4fv(m_vptrans_loc, 1, vptrans);

	glDrawElements(GL_TRIANGLES, m_Keys.size() * 6, GL_UNSIGNED_SHORT, 0);


	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ICSS::graphics::AtlasRenderer::initShader(void)
{
	std::string vShader{
		"uniform mediump vec4 unif_vptrans;"
		"attribute mediump vec4 attr_pos; "
		"attribute mediump vec2 attr_uv;"
		"varying mediump vec2 vary_uv;"
		"void main(){"
		"  gl_Position = attr_pos * unif_vptrans;"
		"  vary_uv = attr_uv;"
		"}"
	};

	std::string fShader{
		"uniform sampler2D unif_texture;"
		"varying mediump vec2 vary_uv;"
		"precision mediump float;"
		"void main(){"
		"  gl_FragColor = texture2D(unif_texture, vary_uv);"
		//"  gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);"
		"}"
	};

	m_shader = GLShaderSet::createFromString(vShader, fShader,
		{
			{ 0, "attr_pos" },
			{ 1, "attr_color" },
		}
	);

	m_shader.use();
	glUniform1i(m_shader.getUniformLocation("unif_texture"), 0);
	m_vptrans_loc = m_shader.getUniformLocation("unif_vptrans");
}

void ICSS::graphics::AtlasRenderer::updateIndices(int start)
{
	int size = m_Keys.size();
	int id = 0;
	uint16_t *ptr = static_cast<uint16_t*>(m_Indices.buffer());
	for(int i = start; i < size; i++)
	{
		id = m_Keys[i].id;
		ptr[0] = id * 4;
		ptr[1] = id * 4 + 1;
		ptr[2] = id * 4 + 2;
		ptr[3] = id * 4 + 1;
		ptr[4] = id * 4 + 3;
		ptr[5] = id * 4 + 2;

		ptr += 6;
	}
	m_Indices.notifyRewrite(start * 6, size * 6 - 1);
}

int ICSS::graphics::AtlasRenderer::positionOffset(void)
{
	return 0;
}

int ICSS::graphics::AtlasRenderer::coordinateOffset(void)
{
	return m_Keys.size() * sizeof(Position2D) * 4;
}

int ICSS::graphics::AtlasRenderer::requiredVBufSize(int atlasCount) const
{
	return atlasCount * (sizeof(Position2D) + sizeof(Coordinate));
}

int ICSS::graphics::AtlasRenderer::requiredIBufSize(int atlasCount) const
{
	return atlasCount * 6 * sizeof(uint16_t);
}

ICSS::graphics::AtlasRenderer::CapacityManager::CapacityManager(void)
{
}

void ICSS::graphics::AtlasRenderer::CapacityManager::resize(int size)
{
	int oldSize = m_spacelist.size();
	
	m_spacelist.resize(size);
	
	if(size > oldSize && oldSize > 0)
	{
		auto end = m_spacelist.begin() + oldSize;
		auto it = std::find(m_spacelist.begin(), end, -1);
		if (it != end)
			*end = oldSize;
	}

	for(int i = oldSize; i < size - 1; i++)
	{
		m_spacelist[i] = i + 1;
	}
	m_spacelist[size - 1] = -1;

	if (m_lastfree == -1)
		m_lastfree = oldSize;

	m_capacity = size;
}

int ICSS::graphics::AtlasRenderer::CapacityManager::obtainSpace(void)
{
	assert(m_lastfree >= -1);
	assert(m_lastfree < m_capacity);

	int curfree = m_lastfree;

	if(curfree != -1)
	{
		m_lastfree = m_spacelist[curfree];
		m_spacelist[curfree] = -2;
	}

	return curfree;
}

void ICSS::graphics::AtlasRenderer::CapacityManager::freeSpace(int id)
{
	if(id >= 0 && id < m_capacity)
	{
		assert(m_spacelist[id] == -2);

		m_spacelist[id] = m_lastfree;
		m_lastfree = id;
	}
}
