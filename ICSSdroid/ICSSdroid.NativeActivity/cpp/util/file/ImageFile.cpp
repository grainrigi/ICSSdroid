#include "pch.h"
#include "util/file/ImageFile.h"

#include "util/file/stb_image.h"
#include "util/file/BinaryFile.h"

using ICSS::file::ImageFile;

using ICSS::container::Uint8Array;

ImageFile ImageFile::loadFromFile(const std::string & fileName, int format)
{
	BinaryFile file = BinaryFile::loadFromFile(fileName);

	return loadFromMemory(file.getData(), file.getSize(), format);
}

ImageFile ICSS::file::ImageFile::loadFromMemory(const uint8_t * buffer, uint32_t size, int format)
{
	ImageFile img;
	img.m_pixels = Uint8Array(reinterpret_cast<uint8_t*>(stbi_load_from_memory(
			reinterpret_cast<const stbi_uc*>(buffer), 
			size, 
			&img.m_x, 
			&img.m_y, 
			&img.m_channels, 
			format
		)),
		img.m_x * img.m_y * img.m_channels, 
		[&](uint8_t *ptr) -> void {
			stbi_image_free(ptr);
		}
	);

	return img;
}

ImageFile::ImageFile(void)
	: m_x(0),
	m_y(0),
	m_channels(0)
{
}
