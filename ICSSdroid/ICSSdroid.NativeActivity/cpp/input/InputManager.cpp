#include "pch.h"
#include "input/InputManager.h"

using namespace ICSS::input;
using namespace ICSS::input::touch;

void InputManager::handleInput(AInputEvent * event)
{
	switch(AInputEvent_getType(event))
	{
		//Touch event handling
		case AINPUT_EVENT_TYPE_MOTION:
		{
			//Reset scanning state
			std::for_each(m_fingers.begin(), m_fingers.end(),
				[](FingerMapValue &v) {
					v.second.first = false;
				});

			
			//read all pointers' information and reflect the state
			int32_t ptrs = AMotionEvent_getPointerCount(event);

			int32_t pid = 0, px = 0, py = 0;
			for(uint32_t i = 0; i < ptrs; i++) {
				pid = AMotionEvent_getPointerId(event, i);
				px = AMotionEvent_getX(event, i);
				py = AMotionEvent_getY(event, i);
				auto finger = m_fingers.find(pid);

				if(finger != m_fingers.end()) {
					finger->second.first = true;
					finger->second.second.updatePosition(px, py);
				}
				else {
					m_fingers.insert(std::make_pair(pid, std::make_pair(true, MotionFinger(px, py))));
				}
			}

			//eliminate unused fingers
			for(auto it = m_fingers.begin(); it != m_fingers.end();) {
				if(!it->second.first)
					it = m_fingers.erase(it);
				else
					it++;
			}
			
		}
	}
}

uint32_t InputManager::addTouchSensor(const touch::TouchSensor::Data & data)
{
	this->m_mtxTsensors.lock();
	this->m_tsensors.insert(std::pair<uint32_t, TouchSensor>(InputManager::obtainID(), TouchSensor(data)));
	this->m_mtxTsensors.unlock();
}

InputManager::InputManager(void)
{
	
}

uint32_t ICSS::input::InputManager::obtainID(void)
{
	static uint32_t id = 0;
	return ++id;
}

void ICSS::input::InputManager::notifyAllSensors(int type, int32_t x, int32_t y)
{
	for(auto it = this->m_tsensors.begin(); it != this->m_tsensors.end(); it++)
		it->second.notifyTouchEvent(type, x, y);
}
