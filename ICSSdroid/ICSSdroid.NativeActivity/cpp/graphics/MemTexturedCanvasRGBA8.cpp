/* 
(c) 2016,2017 Grain

This file is part of ICSSdroid.

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

#include "graphics/MemTexturedCanvasRGBA8.h"

using namespace ICSS::graphics;


ICSS::graphics::MemTexturedCanvasRGBA8::~MemTexturedCanvasRGBA8(void)
{
	if(!m_txunit.expired())
		m_txunit.lock()->ReleaseSpace(m_spaceid);
}

ICSS::graphics::MemTexturedCanvasRGBA8::MemTexturedCanvasRGBA8(void)
	: MemCanvasRGBA8(0, 0, nullptr)
{
}

MemCanvasRGBA8 * ICSS::graphics::MemTexturedCanvasRGBA8::Lock(void)
{
	if (!m_locking)
	{
		m_locking = true;
		return static_cast<MemCanvasRGBA8*>(this);
	}
	else
		return nullptr;
}

void ICSS::graphics::MemTexturedCanvasRGBA8::Unlock(void)
{
	m_locking = false;
	m_txunit.lock()->UpdateTexture(m_spaceid, *static_cast<MemCanvasRGBA8*>(this));
}

void ICSS::graphics::MemTexturedCanvasRGBA8::Resize(int w, int h)
{
	MemCanvasRenderTexturePool *pool = m_pool;
	if (!m_txunit.expired())
		m_txunit.lock()->ReleaseSpace(m_spaceid);
	this->MemCanvasRGBA8::~MemCanvasRGBA8();
	//MemTexturedCanvasRGBA8::operator delete(this, this);
	auto &&c = pool->ObtainCanvas(w, h);
	*this = std::move(c);
}

ICSS::graphics::MemTexturedCanvasRGBA8::MemTexturedCanvasRGBA8(int width, int height)
	: MemCanvasRGBA8(width, height),
	m_locking(false)
{
}
