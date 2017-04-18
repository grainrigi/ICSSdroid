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

#include "graphics/MemSubCanvasRGBA8.h"
#include "graphics/MemCanvasRGBA8.h"

using namespace ICSS::graphics;



MemSubCanvasRGBA8 ICSS::graphics::MemSubCanvasRGBA8::GetSubPixels(int x, int y, int width, int height)
{
	return MemSubCanvasRGBA8(*this, x, y, width, height);
}

ICSS::graphics::MemSubCanvasRGBA8::MemSubCanvasRGBA8(const MemSubCanvasRGBA8 &canvas, int x, int y, int width, int height)
	: MemCanvasRGBA8(width, height, canvas.pixelAt(x, y)),
	m_orgstride { canvas.stride() }
{
}

ICSS::graphics::MemSubCanvasRGBA8::MemSubCanvasRGBA8(const MemCanvasRGBA8 & canvas, int x, int y, int width, int height)
	: MemCanvasRGBA8( width, height, canvas.pixelAt(x, y) ),
	m_orgstride { canvas.stride() }
{
}
