/* 
(c) 2016,2017 Grain

This file is part of ICSSdroid.

ICSEdit is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

ICSEdit is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with ICSEdit.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

namespace ICSS {
namespace graphics {

	class MemCanvas {		
	private:
		int m_channels;
		int m_width;
		int m_height;
		uint8_t *m_pixels;
		bool m_needDelete;
	public:
		MemCanvas(int channels, int width, int height);
		virtual ~MemCanvas(void);

		MemCanvas(const MemCanvas &) = delete;
		MemCanvas &operator=(const MemCanvas&) = delete;
		MemCanvas(MemCanvas &&);
		MemCanvas &operator=(MemCanvas &&);

		virtual int channels(void) const { return m_channels; }
		virtual int width(void) const { return m_width; }
		virtual int height(void) const { return m_height; }
		virtual int stride(void) const { return channels() * width(); }

		virtual void* pixels(void) const { return m_pixels; }
		virtual void* pixelAt(int x, int y) const { return m_pixels + x * channels() + y * stride(); }

	protected:
		MemCanvas(int channels, int width, int height, void *mem);
		void setPixels(void *pixels);
	private:
		
	};

}
}
