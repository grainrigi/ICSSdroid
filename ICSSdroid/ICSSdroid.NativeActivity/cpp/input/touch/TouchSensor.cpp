#include "pch.h"
#include "input/touch/TouchSensor.h"

using namespace ICSS::input::touch;

TouchSensor::TouchSensor(const Data & data)
{
	if(data.left > data.right)
	{
		m_data.left = data.right;
		m_data.right = data.left;
	}
	else
	{
		m_data.left = data.left;
		m_data.right = data.right;
	}

	if(data.top > data.bottom)
	{
		m_data.top = data.bottom;
		m_data.bottom = data.bottom;
	}
	else
	{
		m_data.top = data.top;
		m_data.bottom = data.bottom;
	}

	m_data.queue = data.queue;

}


void TouchSensor::notifyTouchEvent(int type, int32_t x, int32_t y)
{
	TouchNotifyParam notify;
	
	m_x = x;
	m_y = y;

	if(m_data.left <= x && x <= m_data.right)
		if(m_data.top <= y && y <= m_data.bottom) {
			if(type == TouchNotifyParam::ACTION_DOWN) {
				notify.id = 0;
				notify.action = ACTION_DOWN;
				notify.x = x;
				notify.y = y;
				if(this->m_data.queue != nullptr)
					this->m_data.queue->put(notify);
				m_isTouching = true;
			}
			else if(type == TouchNotifyParam::ACTION_UP) {
				notify.id = 0;
				notify.action = ACTION_UP;
				notify.x = x;
				notify.y = y;
				if (this->m_data.queue != nullptr)
					this->m_data.queue->put(notify);
				m_isTouching = false;
			}
			return;
		}
		
	//if finger is out of range
	if(m_isTouching) {
		notify.id = 0;
		notify.action = ACTION_CANCEL;
		notify.x = x;
		notify.y = y;
		if (this->m_data.queue != nullptr)
			this->m_data.queue->put(notify);
		m_isTouching = false;
	}
}
