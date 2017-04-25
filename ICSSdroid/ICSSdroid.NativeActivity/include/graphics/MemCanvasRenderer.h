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
#include "wnd/BoundingBox.h"
#include "graphics/gles/GLShaderSet.h"
#include "graphics/MemCanvasRGBA8.h"
#include "graphics/drawenv.h"
#include "graphics/Mesh2DRenderer.h"
#include "graphics/MemCanvasRenderTexturePool.h"

namespace ICSS {
namespace graphics {

	class MemCanvasRenderer {
		Mesh2DRenderer *m_renderer;
		Mesh2D m_mesh;

		int m_unif_mat_loc;
		MemCanvasRenderTexturePool m_pool;
		gles::GLShaderSet m_shader;

		class DrawUnit
		{
			std::weak_ptr<MemCanvasRenderTexturePool::TextureUnit> m_txunit;
			Mesh2D m_mesh;

		public:
			DrawUnit(std::weak_ptr<MemCanvasRenderTexturePool::TextureUnit> txunit);

			void RegisterElement(int spaceid, int texwidth, int texheight, const wnd::BoundingBox &bbox);
			void DrawRegistered(DrawEnv *env, const MemCanvasRenderer &parent);
		};

		std::unordered_map<int, DrawUnit> m_dunits;
	public:
		MemCanvasRenderer(void);
		MemCanvasRenderer(Mesh2DRenderer *renderer);

		void Render(DrawEnv *env, const MemCanvasRGBA8 &canvas, const wnd::BoundingBox &bb);

		void RegisterElement(MemTexturedCanvasRGBA8 &canvas, const wnd::BoundingBox &bbox, bool autoclip = true);
		void RenderAll(DrawEnv *env);

		MemCanvasRenderTexturePool &GetPool(void) { return m_pool; }

		void LateInitialize(Mesh2DRenderer *renderer);
	private:
		void init(void);
		void initShader(void);
	};

}
}
