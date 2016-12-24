#include "pch.h"
#include "Uint8Array.h"

using ICSS::container::Uint8Array;

ICSS::container::Uint8Array::Uint8Array(void)
	: m_size(0),
	m_array(nullptr),
	m_deleter(nullptr)
{
}

Uint8Array::Uint8Array(uint32_t size)
	: m_size(size),
	m_array(nullptr),
	m_deleter(Uint8Array::default_deleter)
{
	m_array = new uint8_t[size];
}

Uint8Array::Uint8Array(uint8_t * ptr, uint32_t size, std::function<void(uint8_t*)> deleter)
	: m_size(size),
	m_array(ptr),
	m_deleter(deleter)
{
}

Uint8Array::~Uint8Array(void)
{
	if(m_array != nullptr)
	{
		if(m_deleter)
			this->m_deleter(m_array);
		m_array = nullptr;
	}
}

Uint8Array::Uint8Array(const Uint8Array & ary)
	: m_size(ary.m_size),
	m_array(nullptr),
	m_deleter(Uint8Array::default_deleter)
{
	m_array = new uint8_t[m_size];
	memcpy(m_array, ary.m_array, m_size);
}

Uint8Array & Uint8Array::operator=(const Uint8Array & ary)
{
	m_size = ary.m_size;

	if(m_array != nullptr)
		m_deleter(m_array);
	m_deleter = Uint8Array::default_deleter;
	m_array = new uint8_t[m_size];
	memcpy(m_array, ary.m_array, m_size);

	return *this;
}

Uint8Array::Uint8Array(Uint8Array && ary)
	: m_size(ary.m_size),
	m_array(ary.m_array),
	m_deleter(ary.m_deleter)
{
	ary.m_array = nullptr;
}

Uint8Array & Uint8Array::operator=(Uint8Array && ary)
{
	m_size = ary.m_size;

	if (m_array != nullptr)
		m_deleter(m_array);
	m_deleter = ary.m_deleter;
	m_array = ary.m_array;
	ary.m_array = nullptr;

	return *this;
}

void Uint8Array::default_deleter(uint8_t * ptr)
{
	delete ptr;
}
