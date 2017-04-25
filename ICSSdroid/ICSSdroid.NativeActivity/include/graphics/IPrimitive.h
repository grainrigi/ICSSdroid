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
#include "graphics/DrawEnv.h"

namespace ICSS{
namespace graphics{

	class IPrimitive {
	public:
		virtual void draw(DrawEnv *env) = 0;
		virtual ~IPrimitive(void){};
		IPrimitive(void) {};
	protected:
	};

}
}
