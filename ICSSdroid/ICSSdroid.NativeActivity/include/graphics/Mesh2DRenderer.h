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
#include "util/basic/Singleton.h"
#include "graphics/Mesh2D.h"
#include "graphics/gles/GLShaderSet.h"

namespace ICSS{
namespace graphics {

	class Mesh2DRenderer {
		//friend class ICSS::Singleton<Mesh2DRenderer>;

		gles::GLShaderSet m_shader_tex;
		gles::GLShaderSet m_shader_vc;
		gles::GLShaderSet m_shader_ins_gl2_tex;
		gles::GLShaderSet m_shader_ins_gl2_vc;
		gles::GLShaderSet m_shader_ins_gl3_tex;
		gles::GLShaderSet m_shader_ins_gl3_vc;
	public:
		~Mesh2DRenderer(void);
		Mesh2DRenderer(bool init = true);
		
		void LateInitialize(void);

		void draw(DrawEnv *env, Mesh2D &mesh);
		void draw(DrawEnv *env, Mesh2D &mesh, const gles::GLShaderSet &shader, const Mesh2D::ShaderAttributes &attr);
	private:
		

		void initShader(void);

		Mesh2DRenderer(const Mesh2DRenderer&) = delete;
		Mesh2DRenderer& operator=(const Mesh2DRenderer&) = delete;
		Mesh2DRenderer(Mesh2DRenderer&&) = delete;
		Mesh2DRenderer& operator=(Mesh2DRenderer&&) = delete;
	};

}
}