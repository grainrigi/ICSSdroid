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

#include "graphics/MemSubCanvasA8.h"
#include "graphics/MemCanvasA8.h"

using namespace ICSS::graphics;

ICSS::graphics::MemSubCanvasA8::MemSubCanvasA8(const MemCanvasA8 & canvas, int x, int y, int width, int height)
	: MemCanvasA8(width, height, canvas.pixelAt(x, y)),
	m_orgstride{ canvas.stride() }
{
}
