#pragma once

#include "util/threading/IQueueWriter.h"


namespace ICSS {
namespace input{
	class InputManager;
namespace touch{

	struct TouchNotifyParam {
		int type;
		int32_t x;
		int32_t y;
	};

	class TouchSensor {
		friend class ICSS::input::InputManager;

	public:
		struct Data {
			int32_t left;
			int32_t top;
			int32_t right;
			int32_t bottom;
			threading::IQueueWriter<TouchNotifyParam> *queue;
		};

		enum {
			EVENT_DOWN = 1,
			EVENT_UP = 2,
			EVENT_MOVE = 3,
		};

	private:
		//for internal use
		bool m_isTouching = false;
		int32_t m_x;
		int32_t m_y;
		Data m_data;
	public:
		bool getTouching(void) const { return m_isTouching; }
		
		int32_t getTouchX(void) const { return this->m_x; }
		int32_t getTouchY(void) const { return this->m_y; }

		int32_t getLeft(void) const { return this->m_data.left; }
		int32_t getRight(void) const { return this->m_data.right; }
		int32_t getTop(void) const { return this->m_data.top; }
		int32_t getBottom(void) const { return this->m_data.bottom; }

	private:
		//for friend
		TouchSensor(const Data &data);
		void notifyTouchEvent(int type, int32_t x, int32_t y);
	};

}
}
}