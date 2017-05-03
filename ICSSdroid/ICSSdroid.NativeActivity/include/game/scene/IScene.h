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

namespace ICSS {
namespace game {
namespace scene {

	class IScene {
		
	public:
		IScene(void) {};

		virtual void Initialize(void) = 0;

		virtual void Awake(void) = 0;

		virtual void Update(void) = 0;

		virtual void Terminate(void) = 0;
		
	private:
		
	};

}
}
}
