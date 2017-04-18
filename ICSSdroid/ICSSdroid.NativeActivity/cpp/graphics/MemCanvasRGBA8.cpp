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

#include "graphics/MemCanvasRGBA8.h"
#include "graphics/MemSubCanvasRGBA8.h"

using namespace ICSS::graphics;

ICSS::graphics::MemCanvasRGBA8::MemCanvasRGBA8(int width, int height)
	: MemCanvas(4, width, height)
{
}

MemSubCanvasRGBA8 ICSS::graphics::MemCanvasRGBA8::GetSubPixels(int x, int y, int width, int height)
{
	return MemSubCanvasRGBA8(*this, x, y, width, height);
}

void ICSS::graphics::MemCanvasRGBA8::ClearWithColor(uint32_t color)
{
	int x, y, w = width(), h = height(), s = (stride() >> 2) - w;
	uint32_t *pix = reinterpret_cast<uint32_t*>(pixels());
	for(y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++, pix++)
			*pix = color;
		pix += s;
	}
}

void ICSS::graphics::MemCanvasRGBA8::BitBlt(MemCanvasRGBA8 & dst, int dstx, int dsty, int w, int h, MemCanvasRGBA8 & src, int srcx, int srcy)
{
	int dstride = dst.stride();
	int sstride = src.stride();
	uint8_t *dptr = reinterpret_cast<uint8_t*>(dst.pixelAt(dstx, dsty));
	uint8_t *sptr = reinterpret_cast<uint8_t*>(src.pixelAt(srcx, srcy));

	assert(dstx + w <= dst.width());
	assert(srcx + w <= src.width());
	assert(dsty + h <= dst.height());
	assert(srcy + h <= src.height());

	for(int y = 0; y < h; y++)
	{
		memcpy(dptr, sptr, w * sizeof(uint32_t));
		dptr += dstride;
		sptr += sstride;
	}
}

ICSS::graphics::MemCanvasRGBA8::MemCanvasRGBA8(int width, int height, void * mem)
	: MemCanvas(4, width, height, mem)
{
}
