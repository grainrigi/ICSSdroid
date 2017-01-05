#include "pch.h"
#include "input/InputManager.h"

using namespace ICSS::input;
using namespace ICSS::input::touch;

int32_t InputManager::handleInput(AInputEvent * event)
{
	//internal functions
	auto notifyFingerEvent = [&](uint32_t id, int type, int32_t x, int32_t y)
	{
		TouchNotifyParam param = { id, type, x, y };

		for(auto it = m_fingerQueues.begin(); it != m_fingerQueues.end(); it++)
			it->second->put(param);

		for(auto it = m_tsensors.begin(); it != m_tsensors.end(); it++)
			it->second.notifyTouchEvent(id, type, x, y);
	};

	auto notifyFingerEventQueue = [&](uint32_t id, int type, int32_t x, int32_t y)
	{
		TouchNotifyParam param = { id, type, x, y };

		for (auto it = m_fingerQueues.begin(); it != m_fingerQueues.end(); it++)
			it->second->put(param);
	};

	switch(AInputEvent_getType(event))
	{
		//Touch event handling
		case AINPUT_EVENT_TYPE_MOTION:
		{
			int32_t action = AMotionEvent_getAction(event);
			int32_t index = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
			int32_t id = AMotionEvent_getPointerId(event, index); 
			
			switch(action & AMOTION_EVENT_ACTION_MASK) {
				case AMOTION_EVENT_ACTION_DOWN:
				case AMOTION_EVENT_ACTION_POINTER_DOWN:
				{
					//check the id collision and resolve it
					auto it = this->m_fingerIndices.find(id);
					if(it != this->m_fingerIndices.end())
					{
						notifyFingerEvent(it->second.id, TouchNotifyParam::ACTION_UP, -1, -1);
						this->m_fingerIndices.erase(it);
					}

					//attach new id
					uint32_t newUid = InputManager::obtainID();
					FingerData data;
					data.id = newUid;
					data.lastX = AMotionEvent_getX(event, index);
					data.lastY = AMotionEvent_getY(event, index);
					this->m_fingerIndices.insert(std::make_pair(id, data));

					//notify TouchDown event
					notifyFingerEvent(newUid,
						TouchNotifyParam::ACTION_DOWN,
						data.lastX,
						data.lastY
					);

					break;
				}

				//lost all fingers
				case AMOTION_EVENT_ACTION_CANCEL:
				case AMOTION_EVENT_ACTION_UP:
				{
					for(auto it = this->m_fingerIndices.begin(); it != m_fingerIndices.end(); it++)
					{
						if(it->first == id)
							notifyFingerEvent(it->second.id,
								TouchNotifyParam::ACTION_UP,
								AMotionEvent_getX(event, index),
								AMotionEvent_getY(event, index)
							);
						else
							notifyFingerEvent(it->second.id, TouchNotifyParam::ACTION_UP, -1, -1);

					}
					this->m_fingerIndices.clear();
					break;
				}

				case AMOTION_EVENT_ACTION_POINTER_UP:
				{
					auto it = m_fingerIndices.find(id);
					if(it != m_fingerIndices.end())
					{
						notifyFingerEvent(it->second.id,
							TouchNotifyParam::ACTION_UP,
							AMotionEvent_getX(event, index),
							AMotionEvent_getY(event, index)
						);
						m_fingerIndices.erase(it);
					}
					break;
				}

				case AMOTION_EVENT_ACTION_MOVE:
				{
					//scan all pointers
					int32_t count = AMotionEvent_getPointerCount(event);
					int32_t hcount = AMotionEvent_getHistorySize(event);
					for(int32_t i = 0; i < count; i++)
					{
						//check whether the pointer already exists
						int32_t loc_pid = AMotionEvent_getPointerId(event, i);
						int32_t loc_uid;
						auto it = m_fingerIndices.find(loc_pid);
						if(it != m_fingerIndices.end())
							loc_uid = it->second.id;
						else
						{
							loc_uid = InputManager::obtainID();
							it = m_fingerIndices.insert(std::make_pair(loc_pid, FingerData(loc_uid, 0, 0))).first;
						}

						//process historical
						for(int32_t j = 0; j < hcount; j++)
						{
							if (it->second.lastX != AMotionEvent_getHistoricalX(event, i, j) || it->second.lastY != AMotionEvent_getHistoricalY(event, i, j))
							{
								it->second.lastX = AMotionEvent_getHistoricalX(event, i, j);
								it->second.lastY = AMotionEvent_getHistoricalY(event, i, j);
								notifyFingerEventQueue(loc_uid,
									TouchNotifyParam::ACTION_MOVE,
									it->second.lastX,
									it->second.lastY
								);
							}
						}

						//update
						int32_t x = AMotionEvent_getX(event, i);
						int32_t y = AMotionEvent_getY(event, i);
						if(it->second.lastX != x || it->second.lastY != y)
						{
							it->second.lastX = AMotionEvent_getX(event, i);
							it->second.lastY = AMotionEvent_getY(event, i);
							notifyFingerEventQueue(loc_uid,
								TouchNotifyParam::ACTION_MOVE,
								it->second.lastX,
								it->second.lastY
							);
						}
					}
					break;
				}
			}
		}
	}

	return 0;
}

uint32_t InputManager::addTouchSensor(const touch::TouchSensor::Data & data)
{
	uint32_t id = InputManager::obtainID();

	this->m_mtxTsensors.lock();
	this->m_tsensors.insert(std::pair<uint32_t, TouchSensor>(id, TouchSensor(data)));
	this->m_mtxTsensors.unlock();

	return id;
}

void ICSS::input::InputManager::deleteTouchSensor(uint32_t id)
{
	this->m_mtxTsensors.lock();
	this->m_tsensors.erase(id);
	this->m_mtxTsensors.unlock();
}

uint32_t ICSS::input::InputManager::registerFingerListner(threading::IQueueWriter<touch::TouchNotifyParam>* queue)
{
	uint32_t id = InputManager::obtainID();

	this->m_mtxFingerQueues.lock();
	this->m_fingerQueues.insert(std::make_pair(id, queue));
	this->m_mtxFingerQueues.unlock();

	return id;
}

void ICSS::input::InputManager::unregisterFingerListner(uint32_t id)
{
	this->m_mtxFingerQueues.lock();
	this->m_fingerQueues.erase(id);
	this->m_mtxFingerQueues.unlock();
}

InputManager::InputManager(void)
{
	LOGI("InputManager init");
}

uint32_t ICSS::input::InputManager::obtainID(void)
{
	static std::atomic_uint_fast32_t id(0);

	return ++id;
}

void ICSS::input::InputManager::notifyAllSensors(int type, int32_t x, int32_t y)
{
	for(auto it = this->m_tsensors.begin(); it != this->m_tsensors.end(); it++)
		it->second.notifyTouchEvent(0, type, x, y);
}
