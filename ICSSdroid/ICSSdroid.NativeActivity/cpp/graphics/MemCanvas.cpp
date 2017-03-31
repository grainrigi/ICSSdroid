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

#include "graphics/MemCanvas.h"

using namespace ICSS::graphics;

ICSS::graphics::MemCanvas::MemCanvas(int channels, int width, int height)
	: m_channels{channels},
	m_width{width},
	m_height{height},
	m_pixels{new uint8_t[channels * width * height]},
	m_needDelete{true}
{
}

ICSS::graphics::MemCanvas::~MemCanvas(void)
{
	if(m_needDelete)
	{
		delete m_pixels;
		m_needDelete = false;
	}
}

ICSS::graphics::MemCanvas::MemCanvas(MemCanvas &&c)
{
	m_channels = c.m_channels;
	m_width = c.m_width;
	m_height = c.m_height;
	m_pixels = c.m_pixels;
	c.m_pixels = nullptr;
	m_needDelete = c.m_needDelete;
	c.m_needDelete = false;
}

MemCanvas & ICSS::graphics::MemCanvas::operator=(MemCanvas &&c)
{
	m_channels = c.m_channels;
	m_width = c.m_width;
	m_height = c.m_height;
	m_pixels = c.m_pixels;
	c.m_pixels = nullptr;
	m_needDelete = c.m_needDelete;
	c.m_needDelete = false;

	return *this;
}

ICSS::graphics::MemCanvas::MemCanvas(int channels, int width, int height, void * mem)
	: m_channels{channels},
	m_width{width},
	m_height{height},
	m_pixels{reinterpret_cast<uint8_t*>(mem)},
	m_needDelete{false}
{
}

void ICSS::graphics::MemCanvas::setPixels(void * pixels)
{
	if(m_needDelete)
	{
		delete m_pixels;
		m_needDelete = false;
	}
	m_pixels = reinterpret_cast<uint8_t*>(pixels);
}
