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

#include "graphics/MemCanvasA8.h"
#include "graphics/MemSubCanvasA8.h"

using namespace ICSS::graphics;

ICSS::graphics::MemCanvasA8::MemCanvasA8(int width, int height)
	: MemCanvas(1, width, height)
{
}

MemSubCanvasA8 ICSS::graphics::MemCanvasA8::GetSubPixels(int x, int y, int width, int height)
{
	return MemSubCanvasA8(*this, x, y, width, height);
}

void ICSS::graphics::MemCanvasA8::ClearWithColor(uint8_t color)
{
	int w = width(), h = height(), s = stride();
	uint8_t *pix = reinterpret_cast<uint8_t*>(this->pixels());
	for(int y = 0; y < h; y++)
	{
		memset(pix, color, w);
		pix += s;
	}
}

ICSS::graphics::MemCanvasA8::MemCanvasA8(int width, int height, void * mem)
	: MemCanvas(1, width, height, mem)
{
}
