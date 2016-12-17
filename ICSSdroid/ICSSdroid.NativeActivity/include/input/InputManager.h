#pragma once
#include "pch.h"

#include "util/threading/LockFreeQueue8.h"
#include "util/basic/Singleton.h"

#include "./touch/TouchSensor.h"
#include "./touch/TouchNotifyParam.h"



namespace ICSS {
	namespace input {

		class InputManager {
			//This class is only for Singleton
			friend class ICSS::Singleton<InputManager>;

			struct FingerData {
				uint32_t id;
				float lastX;
				float lastY;

				FingerData() = default;
				FingerData(uint32_t id_, int32_t lx_, int32_t ly_) { id = id_; lastX = lx_; lastY = ly_; }
			};

			std::unordered_map<int32_t, FingerData> m_fingerIndices;
			std::unordered_map<uint32_t, touch::TouchSensor> m_tsensors;
			std::unordered_map<uint32_t, threading::IQueueWriter<touch::TouchNotifyParam>*> m_fingerQueues;

			std::mutex m_mtxTsensors;
			std::mutex m_mtxFingerQueues;
		public:
			//Thread Safety : Non-ThreadSafe
			int32_t handleInput(AInputEvent *event);

			//Thread Safety of following 4 functions : ThreadSafe
			uint32_t addTouchSensor(const touch::TouchSensor::Data &data);
			void deleteTouchSensor(uint32_t id);
			uint32_t registerFingerListner(threading::IQueueWriter<touch::TouchNotifyParam> *queue);
			void unregisterFingerListner(uint32_t id);

			//For Debug
			InputManager(void);
		private:
			//InputManager(void);
			static uint32_t obtainID(void);

			void notifyAllSensors(int type, int32_t x, int32_t y);

			//disabled
			/*InputManager(const InputManager&) = delete;
			InputManager &operator=(const InputManager&) = delete;
			InputManager(const InputManager&&) = delete;
			InputManager &operator=(const InputManager&&) = delete;*/
		};

	}
}
