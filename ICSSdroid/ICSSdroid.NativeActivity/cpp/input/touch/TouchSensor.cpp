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
			notify.type = type;
			notify.x = x;
			notify.y = y;
			if(type != TouchSensor::EVENT_MOVE && this->m_data.queue != nullptr)
			    this->m_data.queue->put(notify);
			m_isTouching = (type != EVENT_UP);
			return;
		}
		
	//if finger is out of range
	m_isTouching = false;
}
