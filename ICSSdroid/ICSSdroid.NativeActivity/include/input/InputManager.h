#pragma once
#include "pch.h"

#include "util/threading/LockFreeQueue.h"
#include "util/basic/Singleton.h"

#include "./touch/MotionFinger.h"



namespace ICSS {
	namespace input {
		class InputManager {
			//This class is only for Singleton
			friend class Singleton<InputManager>;

			std::unordered_map<int32_t, touch::MotionFinger> m_fingers;

		public:
			void handleInput(AInputEvent *event);
		private:
			InputManager(void);

			//disabled
			InputManager(const InputManager&) = delete;
			InputManager &operator=(const InputManager&) = delete;
			InputManager(const InputManager&&) = delete;
			InputManager &operator=(const InputManager&&) = delete;
		};
	}
}
