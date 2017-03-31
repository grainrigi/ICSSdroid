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

#include "graphics/Mesh2D.h"
#include "graphics/Mesh2DRenderer.h"

using namespace ICSS::graphics;
using ICSS::graphics::gles::GLVBOTarget;

Mesh2D::Mesh2D(void)
	: m_flags(0),
	m_Position(nullptr),
	m_Coords(nullptr),
	m_Color(nullptr),
	m_Indices(nullptr),
	m_vertCount(0),
	m_elemCount(0),
	m_isVisible(false),
	m_vertBuf(),
	m_idxBuf()
{
}

Mesh2D::Mesh2D(uint8_t attr, GLenum usage)
	: m_flags(attr),
	m_Position(nullptr),
	m_Coords(nullptr),
	m_Color(nullptr),
	m_Indices(nullptr),
	m_vertCount(0),
	m_elemCount(0),
	m_isVisible(true),
	m_vertBuf(GLVBOTarget::ARRAY_BUFFER, usage),
	m_idxBuf(GLVBOTarget::ELEMENT_ARRAY_BUFFER, usage)
{
	if(attr & ATTR_POSITION)
		m_Position = std::make_shared<std::vector<Position>>();
	if (attr & ATTR_COORD)
		m_Coords = std::make_shared<std::vector<Coordinate>>();
	if (attr & ATTR_COLOR)
		m_Color = std::make_shared<std::vector<Color>>();
	if (attr & ATTR_INDEX)
		m_Indices = std::make_shared<std::vector<uint16_t>>();
}

Mesh2D::Mesh2D(uint16_t vertCount, uint8_t attr, GLenum usage)
	: m_flags(attr),
	m_Position(nullptr),
	m_Coords(nullptr),
	m_Color(nullptr),
	m_Indices(nullptr),
	m_vertCount(vertCount),
	m_elemCount(0),
	m_isVisible(true),
	m_vertBuf(GLVBOTarget::ARRAY_BUFFER, usage),
	m_idxBuf(GLVBOTarget::ELEMENT_ARRAY_BUFFER, usage)
{
	if (attr & ATTR_POSITION)
		m_Position = std::make_shared<std::vector<Position>>();
	if (attr & ATTR_COORD)
		m_Coords = std::make_shared<std::vector<Coordinate>>();
	if (attr & ATTR_COLOR)
		m_Color = std::make_shared<std::vector<Color>>();
	if (attr & ATTR_INDEX)
		m_Indices = std::make_shared<std::vector<uint16_t>>();

	this->setVertCount(vertCount);
}

Mesh2D::~Mesh2D(void)
{
}

std::shared_ptr<std::vector<Position>> Mesh2D::positions(void)
{
	dirtyOp();
	if(!(m_flags & ATTR_POSITION) || !this->m_Position)
	{
		this->m_Position = std::make_shared<std::vector<Position>>(m_vertCount);
		this->m_flags |= ATTR_POSITION;
	}
	return this->m_Position;
}

std::shared_ptr<std::vector<Coordinate>> Mesh2D::coordinates(void)
{
	dirtyOp();
	if (!(m_flags & ATTR_COORD) || !this->m_Coords)
	{
		this->m_Coords = std::make_shared<std::vector<Coordinate>>(m_vertCount);
		this->m_flags |= ATTR_COORD;
	}
	return this->m_Coords;
}

std::shared_ptr<std::vector<Color>> Mesh2D::colors(void)
{
	dirtyOp();
	if (!(m_flags & ATTR_COLOR) || !this->m_Color)
	{
		this->m_Color = std::make_shared<std::vector<Color>>(m_vertCount);
		this->m_flags |= ATTR_COLOR;
	}
	return this->m_Color;
}

std::shared_ptr<std::vector<uint16_t>> Mesh2D::indices(void)
{
	dirtyOp();
	if (!(m_flags & ATTR_INDEX) || !this->m_Indices)
	{
		this->m_Indices = std::make_shared<std::vector<uint16_t>>(m_elemCount);
		this->m_flags |= ATTR_INDEX;
	}
	return this->m_Indices;
}

void Mesh2D::unuse(uint8_t attr)
{
	dirtyOp();
	if(attr & ATTR_POSITION)
	{
		this->m_Position = nullptr;
		this->m_flags &= ~ATTR_POSITION;
	}
	if (attr & ATTR_COORD)
	{
		this->m_Coords = nullptr;
		this->m_flags &= ~ATTR_COORD;
	}
	if (attr & ATTR_COLOR)
	{
		this->m_Color = nullptr;
		this->m_flags &= ~ATTR_COLOR;
	}
	if (attr & ATTR_INDEX)
	{
		this->m_Indices = nullptr;
		this->m_flags &= ~ATTR_INDEX;
	}
}

void Mesh2D::setVertCount(uint32_t cnt)
{
	if(m_Position && m_Position->size() < cnt)
		this->m_Position->resize(cnt);
	if(m_Coords && m_Coords->size() < cnt)
		this->m_Coords->resize(cnt);
	if(m_Color && m_Color->size() < cnt)
		this->m_Color->resize(cnt);
	
	this->m_vertCount = cnt;
}

void Mesh2D::setElemCount(uint32_t cnt)
{
	if(m_Indices && m_Indices->size() < cnt*3)
		this->m_Indices->resize(cnt*3);

	this->m_elemCount = cnt;
}

int32_t Mesh2D::positionOffset(void) const
{
	if(m_flags & ATTR_POSITION && m_Position)
		return 0;
	else
		return -1;
}

int32_t Mesh2D::coordOffset(void) const
{
	if (m_flags & ATTR_COORD && m_Coords)
		return positionSize();
	else
		return -1;
}

int32_t Mesh2D::colorOffset(void) const
{
	if (m_flags & ATTR_COLOR && m_Color)
		return positionSize() + coordSize();
	else
		return -1;
}

int32_t Mesh2D::indexOffset(void) const
{
	if (m_flags & ATTR_INDEX && m_Indices)
		return 0;
	else
		return -1;
}

uint32_t Mesh2D::positionSize(void) const
{
	return m_flags & ATTR_POSITION ? std::abs(m_vertCount) * sizeof(Position) : 0;
}

uint32_t Mesh2D::coordSize(void) const
{
	return m_flags & ATTR_COORD ? std::abs(m_vertCount) * sizeof(Coordinate) : 0;
}

uint32_t Mesh2D::colorSize(void) const
{
	return m_flags & ATTR_COLOR ? std::abs(m_vertCount) * sizeof(Color) : 0;
}

uint32_t Mesh2D::indexSize(void) const
{
	return m_flags & ATTR_INDEX ? std::abs(m_elemCount) * sizeof(uint16_t) * 3 : 0;
}

void Mesh2D::upload(void)
{
	uint32_t posSize{0}, coordSize{0}, colorSize{0}, vertCount, idxSize;

	if(m_vertCount <= 0)
		m_vertCount = -(vertCount = (signed int)std::max(
			m_Position ? m_Position->size() : 0, 
			std::max(m_Coords ? m_Coords->size() : 0, 
			m_Color ? m_Color->size() : 0)
		));
	else
		vertCount = m_vertCount;

	if (m_flags & ATTR_POSITION && m_Position)
		posSize = vertCount * sizeof(Position);
	if (m_flags & ATTR_COORD && m_Coords)
		coordSize = vertCount * sizeof(Coordinate);
	if (m_flags & ATTR_COLOR && m_Color)
		colorSize = vertCount * sizeof(Color);
	this->m_vertBuf.extend(posSize + coordSize + colorSize);

	if (posSize)
		m_vertBuf.uploadDataRange(*m_Position, 0, posSize);
	if (coordSize)
		m_vertBuf.uploadDataRange(*m_Coords, posSize, coordSize);
	if (colorSize)
		m_vertBuf.uploadDataRange(*m_Color, posSize + coordSize, colorSize);

	if(m_elemCount <= 0)
	{
		idxSize = m_Indices ? m_Indices->size() * sizeof(uint16_t) : 0;
		m_elemCount = m_Indices ? -(signed int)m_Indices->size() / 3 : 0;
	}
	else
		idxSize = m_elemCount * sizeof(uint16_t) * 3;

	if (m_flags & ATTR_INDEX && m_Indices)
		m_idxBuf.uploadData(*m_Indices, idxSize);

	this->m_flags |= UPLOADED;
}

void Mesh2D::draw(Mesh2DRenderer *renderer, DrawEnv * env)
{
	if(!isUploaded())
		upload();

	renderer->draw(env, *this);
}

void Mesh2D::draw(Mesh2DRenderer *renderer, DrawEnv * env, const gles::GLShaderSet & shader, const ShaderAttributes & attr)
{
	if (!isUploaded())
		upload();

	renderer->draw(env, *this, shader, attr);
}

void Mesh2D::bindVertBuf(void)
{
	this->m_vertBuf.bind();
}

void Mesh2D::bindIdxBuf(void)
{
	this->m_idxBuf.bind();
}


void Mesh2D::checkUploaded(void)
{
	if(!((m_flags & ATTR_POSITION) == 0 || m_Position.unique())
		&& ((m_flags & ATTR_COORD) == 0 || m_Coords.unique())
		&& ((m_flags & ATTR_COLOR) == 0 || m_Color.unique())
		&& ((m_flags & ATTR_INDEX) == 0 || m_Indices.unique()))
		m_flags &= ~UPLOADED;
}
