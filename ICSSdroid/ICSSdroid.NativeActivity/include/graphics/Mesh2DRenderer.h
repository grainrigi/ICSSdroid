#pragma once
#include "util/basic/Singleton.h"
#include "graphics/Mesh2D.h"
#include "graphics/gles/GLShaderSet.h"

namespace ICSS{
namespace graphics {

	class Mesh2DRenderer {
		friend class ICSS::Singleton<Mesh2DRenderer>;

		gles::GLShaderSet m_shader_tex;
		gles::GLShaderSet m_shader_vc;
		gles::GLShaderSet m_shader_ins_gl2_tex;
		gles::GLShaderSet m_shader_ins_gl2_vc;
		gles::GLShaderSet m_shader_ins_gl3_tex;
		gles::GLShaderSet m_shader_ins_gl3_vc;
	public:
		~Mesh2DRenderer(void);

		

		void draw(DrawEnv *env, Mesh2D &mesh);
		void draw(DrawEnv *env, Mesh2D &mesh, const gles::GLShaderSet &shader, const Mesh2D::ShaderAttributes &attr);
	private:
		Mesh2DRenderer(void);

		void initShader(void);

		Mesh2DRenderer(const Mesh2DRenderer&) = delete;
		Mesh2DRenderer& operator=(const Mesh2DRenderer&) = delete;
		Mesh2DRenderer(Mesh2DRenderer&&) = delete;
		Mesh2DRenderer& operator=(Mesh2DRenderer&&) = delete;
	};

}
}