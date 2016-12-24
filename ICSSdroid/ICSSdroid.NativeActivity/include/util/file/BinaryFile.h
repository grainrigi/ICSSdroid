#pragma once
#include "pch.h"
#include "util/container/Uint8Array.h"

namespace ICSS{
namespace file{

	class BinaryFile {
	protected:
		ICSS::container::Uint8Array m_data;

	public:
		static BinaryFile loadFromFile(const std::string &fileName);

		BinaryFile(void);
		~BinaryFile(void) = default;

		BinaryFile(const BinaryFile &file) = default;
		BinaryFile &operator=(const BinaryFile &file) = default;
		BinaryFile(BinaryFile &&file) = default;
		BinaryFile &operator=(BinaryFile &&file) = default;

		virtual uint32_t getSize(void) const { return this->m_data.getSize(); }
		virtual const uint8_t *getData(void) const { return this->m_data.getPtr(); }
	private:
		
	};

}
}