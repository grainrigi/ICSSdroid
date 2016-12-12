#pragma once
#include "pch.h"

namespace ICSS {
namespace input {
	class InputManager;
namespace touch {

	class MotionFinger {
		friend class ICSS::input::InputManager;

		uint32_t m_id;
	public:
		struct Data {
			int32_t prevX;
			int32_t prevY;
			int32_t x;
			int32_t y;
		};
	private:
		Data m_pos = {0};

	public:
		

		uint32_t getID(void) const { return m_id; }

		int getX(void) const { return m_pos.x; }
		int getY(void) const { return m_pos.x; }
		int getPrevX(void) const { return m_pos.prevX; }
		int getPrevY(void) const { return m_pos.prevY; }
		void getPos(MotionFinger::Data *buf) const { memcpy(buf, &m_pos, sizeof(MotionFinger::Data)); }
	private:
		//only InputManager can instantiate
		MotionFinger(void);
		MotionFinger(int32_t x, int32_t y);

		/*** static methods ***/
		static uint32_t obtainID(void);
		
		void updatePosition(int32_t x, int32_t y);
		
	};

}
}
}