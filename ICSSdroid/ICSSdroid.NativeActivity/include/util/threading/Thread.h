#pragma once
#include "pch.h"
#include "util/basic/IDisposable.h"
#include <atomic>


namespace ICSS {
namespace threading {

	class Thread : public IDisposable {
		std::thread m_th;
		std::atomic_bool m_isTerminating;
		void *m_data = nullptr;

	public:
		Thread(void) : m_isTerminating(false) { }

		Thread(std::function<void(Thread*)> &&action, void *data) :
			m_isTerminating(false)
		{
			this->m_data = data;
			m_th = std::thread(std::forward<std::function<void(Thread*)>>(action), this);
		}

		~Thread(void) 
		{
			this->i_terminate();
		}

		void startThread(std::function<void(Thread*)> &&action, void *data) 
		{
			this->i_terminate();
			m_isTerminating = false;

			this->m_data = data;
			m_th = std::thread(std::forward<std::function<void(Thread*)>>(action), this);
		}

		//Should be called by the thread
		bool isTerminating(void) { return this->m_isTerminating; }
	
		//Should be called by the thread creator
		bool waitForTerminate(void) { return this->i_terminate(); }

	
		//IDisposable
		virtual void Dispose(void) { this->i_terminate(); }

	private:
		bool i_terminate() {
			bool expected = false;

			m_isTerminating.compare_exchange_strong(expected, true);
			if(!expected && this->m_th.joinable()) {
				this->m_th.join();
				return true;
			}
			else
				return false;
		}
	};

	class ThreadManager {

	};

}
}