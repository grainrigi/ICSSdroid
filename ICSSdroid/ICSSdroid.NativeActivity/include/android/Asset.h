#pragma once
#include "util/container/Uint8Array.h"
#include "util/file/BinaryFile.h"

namespace ICSS{
namespace android{
	class AssetManager;
	class Asset : public ICSS::file::BinaryFile {
		friend class ICSS::android::AssetManager;

	public:
		Asset(void) {}
		~Asset(void) = default;

		Asset(const Asset &asset) = default;
		Asset& operator=(const Asset &asset) = default;
		Asset(Asset &&asset) = default;
		Asset& operator=(Asset &&asset) = default;

		uint32_t getSize(void) const { return m_data.getSize(); }
		const uint8_t *getData(void) const { return m_data.getPtr(); }
	private:
		Asset(AAsset *asset);
	};

}
}