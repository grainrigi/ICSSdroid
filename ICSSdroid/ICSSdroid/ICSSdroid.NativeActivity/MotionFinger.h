#pragma once
#include "pch.h"

namespace ICSS {
	class InputManager;
	class MotionFinger {
		friend class InputManager;

		uint32_t m_id;
	public:
		struct Data {
			int prevX;
			int prevY;
			int x;
			int y;
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

		/*** static methods ***/
		static uint32_t obtainID(void);
		
		void updatePosition(int x, int y);
		
	};
}