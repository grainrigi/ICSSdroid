/*
(c) 2016,2017 Grain

This file is part of ICSEdit.

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