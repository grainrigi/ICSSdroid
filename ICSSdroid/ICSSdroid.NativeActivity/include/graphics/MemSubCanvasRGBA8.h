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

#pragma once
#include "graphics/MemCanvas.h"
#include "graphics/MemCanvasRGBA8.h"

namespace ICSS {
namespace graphics {

	class MemSubCanvasRGBA8 : public MemCanvasRGBA8 {
		friend class MemCanvasRGBA8;

		int m_orgstride;
	public:
		virtual ~MemSubCanvasRGBA8(void) {};
		MemSubCanvasRGBA8(MemSubCanvasRGBA8 &&) = default;
		MemSubCanvasRGBA8 &operator=(MemSubCanvasRGBA8&&) = default;

		virtual int stride(void) const override { return m_orgstride; }

		virtual MemSubCanvasRGBA8 GetSubPixels(int x, int y, int width, int height) override;

	private:
		MemSubCanvasRGBA8(const MemSubCanvasRGBA8 &canvas, int x, int y, int width, int height);
		MemSubCanvasRGBA8(const MemCanvasRGBA8 &canvas, int x, int y, int width, int height);
	};

}
}
