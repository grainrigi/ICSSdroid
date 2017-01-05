#pragma once
#include "pch.h"

namespace ICSS{
namespace graphics{
namespace gles{

	class GLVBO {
		GLuint m_buffer;
		GLenum m_target;
		GLsizeiptr m_size;
		GLenum m_usage;
		bool m_isMapping;

	public:
		GLVBO(void);
		GLVBO(GLenum target, GLenum usage);
		GLVBO(GLenum target, GLsizeiptr size, GLenum usage);
		~GLVBO(void);

		GLVBO(const GLVBO &vbo) = delete;
		GLVBO &operator=(const GLVBO &vbo) = delete;
		GLVBO(GLVBO &&vbo);
		GLVBO &operator=(GLVBO &&vbo);

		
		template<typename _Ty>
		void uploadData(const std::vector<_Ty> &vec)
		{
			uploadData(vec, 0);
		}
		template<typename _Ty>
		void uploadData(const std::vector<_Ty> &vec, uint32_t maxSize) {
			GLsizeiptr size;
			if(maxSize == 0)
				size = vec.size() * sizeof(_Ty);
			else
				size = vec.size() * sizeof(_Ty) < maxSize ? vec.size() * sizeof(_Ty) : maxSize;

			if (size > this->m_size)
			{
				glBindBuffer(m_target, m_buffer);
				glBufferData(m_target, size, (void*)&vec[0], m_usage);
				this->m_size = size;
			}
			else
				uploadDataRange(vec, 0, maxSize);
		}
		void uploadData(GLsizeiptr size, const GLvoid * data);

		template<typename _Ty>
		void uploadDataRange(const std::vector<_Ty> &vec, GLuint offset) {
			GLsizeiptr size = vec.size() * sizeof(_Ty);

			extend(size + offset, true);

			glBindBuffer(m_target, m_buffer);
			glBufferSubData(m_target, offset, size, (void*)&vec[0]);
		}
		template<typename _Ty>
		void uploadDataRange(const std::vector<_Ty> &vec, GLuint offset, uint32_t maxSize) {
			GLsizeiptr size;
			if (maxSize == 0)
				size = vec.size() * sizeof(_Ty);
			else
				size = vec.size() * sizeof(_Ty) < maxSize ? vec.size() * sizeof(_Ty) : maxSize;

			extend(size + offset, true);

			glBindBuffer(m_target, m_buffer);
			glBufferSubData(m_target, offset, size, (void*)&vec[0]);
		}
		void uploadDataRange(GLsizeiptr size, const GLvoid * data, GLuint offset);

		GLsizeiptr size(void) const { return this->m_size; }
		void extend(GLsizeiptr size, bool preserve = false);
		void updateUsage(GLenum usage) { this->m_usage = usage; }

		/*
		void *map(GLbitfield access);
		void *map(GLintptr offset, GLsizeiptr size, GLbitfield access);
		void unmap(void);
		*/

		void bind(void);
	};

}
}
}