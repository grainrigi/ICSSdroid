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

namespace ICSS {
namespace wnd {

	class BoundingBox {
		int x, y;
		int w, h;
	public:
		BoundingBox(void);
		BoundingBox(int x, int y, int w, int h);

		int GetX(void) const { return x; }
		int GetY(void) const { return y; }
		int GetWidth(void) const { return w; }
		int GetHeight(void) const { return h; }

		void SetX(int nx) { x = nx; }
		void SetY(int ny) { y = ny; }
		void SetWidth(int nw);
		void SetHeight(int nh);

		bool isIncludePoint(int x, int y) const;
		
	private:
		
	};

}
}
