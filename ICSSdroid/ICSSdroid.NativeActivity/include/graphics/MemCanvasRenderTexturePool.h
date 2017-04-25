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
#include "graphics/gles/GLTexture.h"
#include "graphics/MemCanvasRGBA8.h"

namespace ICSS {
namespace graphics {
	class MemTexturedCanvasRGBA8;

	class MemCanvasRenderTexturePool {
	public:
		static constexpr int BLOCK_WIDTH = 64;
		static constexpr int BLOCK_HEIGHT = 64;
		static constexpr int BLOCK_HORZ_COUNT = 32;
		static constexpr int BLOCK_VERT_COUNT = 32;

		static constexpr int textureWidth(void) { return BLOCK_WIDTH * BLOCK_HORZ_COUNT; }
		static constexpr int textureHeight(void) { return BLOCK_HEIGHT * BLOCK_VERT_COUNT; }

		

	public:
		MemCanvasRenderTexturePool(void);

		virtual MemTexturedCanvasRGBA8 ObtainCanvas(int width, int height);

		struct SpaceUnit
		{
			int id;
			int x, y;
			int w, h;
		};

		class TextureUnit
		{
			int m_id;
			gles::GLTexture m_texture;
			uint8_t m_map[BLOCK_HORZ_COUNT * BLOCK_VERT_COUNT];
			std::unordered_map<int, SpaceUnit> m_spaces;

		public:
			TextureUnit(void);
			TextureUnit(uint32_t obtained_id);

			int id(void) const { return m_id; }
			GLuint texhandle(void) const { return m_texture.handle(); }
			SpaceUnit ObtainNewSpace(int width, int height);
			void ReleaseSpace(int spaceid);
			void UpdateTexture(int spaceid, const MemCanvasRGBA8 &canvas);

			const SpaceUnit &GetSpaceInfo(int spaceid);
		};

		std::shared_ptr<TextureUnit> GetTxUnit(uint32_t id);
		

	private:
		std::unordered_map<uint32_t, std::shared_ptr<TextureUnit>> m_units;
		static int ObtainNewID(void);
		
	};

}
}
