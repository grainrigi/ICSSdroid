/* 
(c) 2016,2017 Grain

This file is part of ICSSdroid.

ICSSdroid is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

ICSSdroid is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with ICSSdroid.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include "graphics/MemCanvasRGBA8.h"
#include "graphics/MemCanvasRenderTexturePool.h"

namespace ICSS {
namespace graphics {
	class MemCanvasRenderer;
	class MemCanvasRenderTexturePool;

	class MemTexturedCanvasRGBA8 : private MemCanvasRGBA8 {
		friend class MemCanvasRenderer;
		friend class MemCanvasRenderTexturePool;

		MemCanvasRenderTexturePool *m_pool;
		std::weak_ptr<MemCanvasRenderTexturePool::TextureUnit> m_txunit;
		int m_spaceid;
		bool m_locking;
	public:
		~MemTexturedCanvasRGBA8(void);
		static void operator delete (void *p, void *buf) {};
		static void operator delete (void *p) {};
		
		MemTexturedCanvasRGBA8(void);

		MemTexturedCanvasRGBA8(MemTexturedCanvasRGBA8 &&) = default;
		MemTexturedCanvasRGBA8 &operator=(MemTexturedCanvasRGBA8 &&) = default;

		virtual int channels(void) const { return MemCanvasRGBA8::channels(); }
		virtual int width(void) const { return MemCanvasRGBA8::width(); }
		virtual int height(void) const { return MemCanvasRGBA8::height(); }
		virtual int stride(void) const { return channels() * width(); }

		uint32_t txunitid(void) const { return m_txunit.expired() ? 0 : m_txunit.lock()->id(); }
		uint32_t spaceid(void) const { return m_spaceid; }

		MemCanvasRGBA8 *Lock(void);
		void Unlock(void);

		void Resize(int w, int h);
		
	private:
		MemTexturedCanvasRGBA8(int width, int height);
	};

}
}
