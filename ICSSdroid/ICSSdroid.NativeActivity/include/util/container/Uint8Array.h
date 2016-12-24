#pragma once
#include "pch.h"

namespace ICSS{
namespace container{

	class Uint8Array {
		uint32_t m_size;
		uint8_t *m_array;
		std::function<void(uint8_t*)> m_deleter;

	public:
		Uint8Array(void);
		Uint8Array(uint32_t size);
		Uint8Array(uint8_t *ptr, uint32_t size, std::function<void(uint8_t*)> deleter);
		~Uint8Array(void);

		Uint8Array(const Uint8Array &ary);
		Uint8Array &operator=(const Uint8Array &ary);
		Uint8Array(Uint8Array &&ary);
		Uint8Array &operator=(Uint8Array &&ary);

		uint32_t getSize(void) const { return m_size; }
		uint8_t *getPtr(void) const { return this->m_array; }
	private:
		static void default_deleter(uint8_t *ptr);
	};

}
}