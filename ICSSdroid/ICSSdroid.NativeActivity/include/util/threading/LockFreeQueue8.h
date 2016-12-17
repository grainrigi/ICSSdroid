#pragma once
#include "pch.h"
#include "util/basic/IDisposable.h"
#include <atomic>


#include "IQueueReader.h"
#include "IQueueWriter.h"

#define MINUS_ONE 0xFF


namespace ICSS {
namespace threading {
	template<typename _Ty>
	class LockFreeQueue8 : public IDisposable, public IQueueReader<_Ty>, public IQueueWriter<_Ty> {
		_Ty *m_buffer = nullptr;
		uint32_t m_size = 0;
		std::atomic_uint_fast8_t m_ridx;
		std::atomic_uint_fast8_t m_widx;
		std::atomic_uint_fast8_t m_count;
		std::atomic_uint_fast8_t m_reading;
		std::atomic_bool *m_ready = nullptr;

	public:
		//Argument
		// capacity : the size of queue, which must be above 0
		//Exception
		// invalid_argument : if the capacity is or below 0
		LockFreeQueue8(uint8_t capacity) : 
			m_ridx(0),
			m_widx(0),
			m_count(0),
			m_reading(0)
		{
			/*
			if(capacity == 0)
				throw std::invalid_argument("capacity must be above 0.");
				*/
			m_size = capacity;
			//init buffer
			m_buffer = (_Ty*)new uint8_t[sizeof(_Ty) * capacity];
			//init ready state
			m_ready = new std::atomic_bool[capacity];
			for(uint8_t i = 0; i < capacity; i++) {
				m_ready[i].store(false);
			}
		}

		~LockFreeQueue8(void) {
			this->Dispose();
		}

		//Thread safety : ThreadSafe
		//Return Value
		// true : the data was safely read
		// false : there was no data that could be read
		virtual bool get(_Ty *buffer) override {
			uint8_t idx;

			if((idx = popIndex()) == MINUS_ONE)
				return false;

			memcpy(buffer, this->m_buffer + idx, sizeof(_Ty));
			this->m_ready[idx].store(false);
			return true;
		}

		//Thread safety : ThreadSafe
		//Return Value
		// true : the data was safely put
		// false : there was no capacity to put the data
		virtual bool put(const _Ty &data) override
		{
			uint8_t idx;

			if((idx = reserveWriteSpace()) == MINUS_ONE)
				return false;

			memcpy(this->m_buffer + idx, &data, sizeof(_Ty));
			this->m_ready[idx].store(true);
			return true;
		}


		//IDisposable
		// Thread safety : Non-ThreadSafe
		virtual void Dispose(void) override {
			if (m_buffer != nullptr) {
				delete m_buffer;
				m_buffer = nullptr;
			}
			if (m_ready != nullptr) {
				delete m_ready;
				m_ready = nullptr;
			}
		}

		LockFreeQueue8(LockFreeQueue8&&) = default;
		LockFreeQueue8 &operator=(LockFreeQueue8&&) = default;
	private:
		uint8_t popIndex(void) 
		{
			uint8_t ridx;
			uint8_t newridx;
			uint8_t ctr;
			
			//update counter safely
			while (1) {
				ctr = m_count;
				if (ctr <= 0)
					//buffer is empty
					return MINUS_ONE;

				//decrease the count
				if (m_count.compare_exchange_weak(ctr, ctr - 1))
					break;
			}

			//update read index
			while(1) {
				ridx = m_ridx;
				if(!m_ready[ridx])
				{
					m_count++;
					return false;
				}
				newridx = (ridx + 1) % m_size;
				if(m_ridx.compare_exchange_weak(ridx, newridx))
					break;
			}

			return ridx;
		}

		uint8_t reserveWriteSpace(void) 
		{
			uint8_t widx;
			uint8_t newwidx;
			uint8_t ctr;

			//update counter safely
			while(1) {
				ctr = m_count;
				if(ctr >= m_size - 1)
					//buffer is full
					return MINUS_ONE;

				//increase the count
				if(m_count.compare_exchange_weak(ctr, ctr + 1))
					break;
			}

			//update write index
			while(1) {
				widx = m_widx;
				if(m_ready[widx])
				{
					m_count--;
					return MINUS_ONE;
				}

				newwidx = (widx + 1) % m_size;
				if(m_widx.compare_exchange_weak(widx, newwidx))
					break;
			}

			return widx;
		}

		//disabled
		LockFreeQueue8(const LockFreeQueue8&) = delete;
		LockFreeQueue8 &operator=(const LockFreeQueue8&) = delete;
	};
}
}