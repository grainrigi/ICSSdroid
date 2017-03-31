#pragma once
#include "pch.h"

namespace ICSS{
namespace graphics{
namespace gles {
	class GLShaderSet {
		GLuint m_program;
		GLuint m_vShader;
		GLuint m_fShader;

	public:
		GLShaderSet(void);
		static GLShaderSet createFromString(const std::string &vShader, const std::string &fShader, const std::map<GLuint, std::string> &attribs);
		~GLShaderSet(void);

		GLShaderSet(const GLShaderSet &ary) = delete;
		GLShaderSet &operator=(const GLShaderSet &ary) = delete;
		GLShaderSet(GLShaderSet &&ary);
		GLShaderSet &operator=(GLShaderSet &&ary);

		GLuint program(void) const { return m_program; }
		GLuint vShader(void) const { return m_vShader; }
		GLuint fShader(void) const { return m_fShader; }

		void use(void) { glUseProgram(m_program); }

		GLint getAttribLocation(const std::string &str) const { return glGetAttribLocation(m_program, str.c_str()); }
		GLint getUniformLocation(const std::string &str) const { return glGetUniformLocation(m_program, str.c_str()); }

		bool operator==(const GLShaderSet &set) { return m_program == set.m_program; }
	private:
		GLuint createShader(GLenum type, const std::string &src);
		GLuint createProgram(void);
		void linkProgram(void);
	};
}
}
}