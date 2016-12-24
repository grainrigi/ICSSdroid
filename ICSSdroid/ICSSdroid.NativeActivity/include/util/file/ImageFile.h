#pragma once
#include "pch.h"
#include "util/container/Uint8Array.h"

namespace ICSS{
namespace file{

	class ImageFile {
		int32_t m_x;
		int32_t m_y;
		int32_t m_channels;
		ICSS::container::Uint8Array m_pixels;

	public:
		enum {
			FORMAT_RGB = 3,
			FORMAT_RGBA = 4,
		};

		//The file size is limited up to 4GiB
		static ImageFile loadFromFile(const std::string &fileName, int format);
		static ImageFile loadFromMemory(const uint8_t *buffer, uint32_t size, int format);
		~ImageFile(void) = default;

		ImageFile(const ImageFile &img) = default;
		ImageFile &operator=(const ImageFile &img) = default;
		ImageFile(ImageFile &&img) = default;
		ImageFile &operator=(ImageFile &&img) = default;

		int32_t getX(void) const { return this->m_x; }
		int32_t getY(void) const { return this->m_y; }
		int32_t getChannelCount(void) const { return this->m_channels; }

		uint32_t getPixelsSize(void) const { return this->m_pixels.getSize(); }
		const uint8_t *getPixels(void) const { return this->m_pixels.getPtr(); }

	private:
		ImageFile(void);
	};

}
}
