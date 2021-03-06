#include "pch.h"
#include "input/touch/MotionFinger.h"

using namespace ICSS::input::touch;

MotionFinger::MotionFinger(void)
{
	m_id = MotionFinger::obtainID();
}

MotionFinger::MotionFinger(int32_t x, int32_t y)
{
	m_id = MotionFinger::obtainID();
	updatePosition(x, y);
}

uint32_t MotionFinger::obtainID(void)
{
	static uint32_t id = 0;
	return id++;
}

void MotionFinger::updatePosition(int32_t x, int32_t y)
{
	m_pos.prevX = m_pos.x;
	m_pos.prevY = m_pos.y;
	m_pos.x = x;
	m_pos.y = y;
}

