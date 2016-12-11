#pragma once
#include "pch.h"
#include "LockFreeQueue.h"
#include "Singleton.h"
#include "MotionFinger.h"



namespace ICSS {
	class InputManager {
		//This class is only for Singleton
		friend class Singleton<InputManager>;

		std::unordered_map<int32_t, MotionFinger> m_fingers;

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
