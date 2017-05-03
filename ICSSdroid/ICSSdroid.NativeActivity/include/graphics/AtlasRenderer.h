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

#include "graphics/Geometry.h"
#include "graphics/gles/GLVBO.h"
#include "graphics/gles/GLShaderSet.h"

namespace ICSS {
namespace graphics {

	struct AtlasVertex {

	};

	class AtlasRenderer {

		class CapacityManager
		{
			int m_capacity = 0;
			int m_lastfree = -1;
			std::vector<uint32_t> m_spacelist;
		public:
			CapacityManager(void);

			void resize(int size);

			int obtainSpace(void);
			void freeSpace(int id);
		};

		template <typename Elem>
		class RangeManagedVBO
		{
			int m_start = 0;
			int m_end = -1;

			std::vector<Elem> m_Elements;

		public:
			RangeManagedVBO(void) {};

			void resize(int size);
			int size(void) const { return m_Elements.size(); }

			void notifyRewrite(int start, int end);
			void resetRange(void);

			int start(void) const { return m_start; }
			int end(void) const { return m_end; }

			void* buffer(void) const { return (void*)&m_Elements[0]; }

			void upload(GLenum target, int offset);
		};

		struct KeyBoundIndex{
			int id;
			float key;

			bool operator<(const KeyBoundIndex &index) const
			{
				return this->key < index.key;
			}
		};

		int m_vptrans_loc;
		gles::GLShaderSet m_shader;
		gles::GLVBO m_vbuf;
		gles::GLVBO m_ibuf;

		int m_atlasCapacity = 0;
		CapacityManager m_capacity;
		RangeManagedVBO<Position2D> m_Positions;
		RangeManagedVBO<Coordinate> m_Coordinates;

		int m_IndicesUpdateStart = -1;
		RangeManagedVBO<uint16_t> m_Indices;
		std::vector<KeyBoundIndex> m_Keys;

	public:
		AtlasRenderer(int count = 100);

		void atlasCapacity(int capacity);

		void updatePosition(int id, float x, float y, float w, float h);
		void updateCoordinate(int id, float u_lt, float v_lt, float u_rb, float v_rb);

		int addAtlas(float key);
		void removeAtlas(int id);

		void drawAll(float width, float height);
		
	private:
		void initShader(void);
		void updateIndices(int start);

		int positionOffset(void);
		int coordinateOffset(void);
		int requiredVBufSize(int atlasCount) const;
		int requiredIBufSize(int atlasCount) const;
	};

	template<typename Elem>
	inline void AtlasRenderer::RangeManagedVBO<Elem>::resize(int size)
	{
		m_Elements.resize(size);
	}

	template<typename Elem>
	inline void AtlasRenderer::RangeManagedVBO<Elem>::notifyRewrite(int start, int end)
	{
		assert(start >= 0);
		assert(end < this->size());
		assert(end >= start);

		if (start < m_start)
			m_start = start;
		if (end > m_end)
			m_end = end;
	}

	template<typename Elem>
	inline void AtlasRenderer::RangeManagedVBO<Elem>::resetRange(void)
	{
		m_start = 0;
		m_end = -1;
	}

	template<typename Elem>
	inline void AtlasRenderer::RangeManagedVBO<Elem>::upload(GLenum target, int offset)
	{
		if (m_end - m_start + 1 > 0)
		{
			glBufferSubData(target,
				offset + m_start * sizeof(Elem),
				(m_end - m_start + 1) * sizeof(Elem),
				static_cast<uint8_t*>(this->buffer()) + m_start * sizeof(Elem));
		}

		this->resetRange();
	}

}
}
