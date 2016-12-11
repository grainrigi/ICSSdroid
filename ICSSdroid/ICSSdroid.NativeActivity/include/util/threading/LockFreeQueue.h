#pragma once
#include "pch.h"
#include "util/basic/IDisposable.h"
#include <atomic>


namespace ICSS {
	namespace threading {
		template<typename _Ty>
		class LockFreeQueue : public IDisposable {
			_Ty *m_buffer = nullptr;
			uint32_t m_size = 0;
			std::atomic_uint_fast32_t m_ridx;
			std::atomic_uint_fast32_t m_widx;
			std::atomic_uint_fast32_t m_count;
			std::atomic_uint_fast32_t m_reading;
			std::atomic_bool *m_ready = nullptr;

		public:
			//Argument
			// capacity : the size of queue, which must be above 0
			//Exception
			// invalid_argument : if the capacity is or below 0
			LockFreeQueue(uint32_t capacity) : 
				m_ridx(0),
				m_widx(0),
				m_count(0),
				m_reading(0)
			{
				/*
				if(capacity == 0)
					throw std::invalid_argument("capacity must be above 0.");
					*/
				m_size = capacity + 1;
				//init buffer
				m_buffer = (_Ty*)new uint8_t[sizeof(_Ty) * (capacity + 1)];
				//init ready state
				m_ready = new std::atomic_bool[capacity + 1];
				for(uint32_t i = 0; i < capacity + 1; i++) {
					m_ready[i].store(false);
				}
			}

			~LockFreeQueue(void) {
				this->Dispose();
			}

			//Thread safety : ThreadSafe
			//Return Value
			// true : the data was safely read
			// false : there was no data that could be read
			bool get(_Ty *buffer) {
				uint32_t idx;

				if((idx = popIndex()) == -1)
					return false;

				memcpy(buffer, this->m_buffer + idx, sizeof(_Ty));
				this->m_ready[idx].store(false);
				return true;
			}

			//Thread safety : ThreadSafe
			//Return Value
			// true : the data was safely put
			// false : there was no capacity to put the data
			bool put(const _Ty &data) 
			{
				uint32_t idx;

				if((idx = reserveWriteSpace()) == -1)
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
		private:
			uint32_t popIndex(void) 
			{
				uint32_t ridx;
				uint32_t newridx;
				uint32_t ctr;
			
				//update counter safely
				while (1) {
					ctr = m_count;
					if (ctr <= 0)
						//buffer is empty
						return -1;

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

			uint32_t reserveWriteSpace(void) 
			{
				uint32_t widx;
				uint32_t newwidx;
				uint32_t ctr;

				//update counter safely
				while(1) {
					ctr = m_count;
					if(ctr >= m_size - 1)
						//buffer is full
						return -1;

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
						return -1;
					}

					newwidx = (widx + 1) % m_size;
					if(m_widx.compare_exchange_weak(widx, newwidx))
						break;
				}

				return widx;
			}
		};
	}
}