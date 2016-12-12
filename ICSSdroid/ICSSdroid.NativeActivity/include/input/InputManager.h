#pragma once
#include "pch.h"

#include "util/threading/LockFreeQueue8.h"
#include "util/basic/Singleton.h"

#include "./touch/MotionFinger.h"
#include "./touch/TouchSensor.h"



namespace ICSS {
	namespace input {

		class InputManager {
			//This class is only for Singleton
			friend class ICSS::Singleton<InputManager>;

			typedef std::unordered_map<int32_t, std::pair<bool, touch::MotionFinger>> FingerMap;
			typedef FingerMap::value_type FingerMapValue;

			FingerMap m_fingers;
			std::unordered_map<uint32_t, touch::TouchSensor> m_tsensors;

			std::mutex m_mtxTsensors;
		public:
			void handleInput(AInputEvent *event);

			uint32_t addTouchSensor(const touch::TouchSensor::Data &data);
		private:
			InputManager(void);
			static uint32_t obtainID(void);

			void notifyAllSensors(int type, int32_t x, int32_t y);

			//disabled
			InputManager(const InputManager&) = delete;
			InputManager &operator=(const InputManager&) = delete;
			InputManager(const InputManager&&) = delete;
			InputManager &operator=(const InputManager&&) = delete;
		};

	}
}
