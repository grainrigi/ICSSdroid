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

#pragma once
#include "graphics/MemCanvas.h"
#include "graphics/MemCanvasA8.h"

namespace ICSS {
namespace graphics {
	

	class MemSubCanvasA8 : public MemCanvasA8 {
		friend class MemCanvasA8;

		int m_orgstride;
	public:
		MemSubCanvasA8() = delete;
		virtual ~MemSubCanvasA8(void) {};

		MemSubCanvasA8(MemSubCanvasA8&&) = default;
		MemSubCanvasA8 &operator=(MemSubCanvasA8&&) = default;

		virtual int stride(void) const override { return m_orgstride; }

	private:
		MemSubCanvasA8(const MemCanvasA8 &canvas, int x, int y, int width, int height);
	};

}
}
