#pragma once
#include "pch.h"
#include "graphics/Geometry.h"
#include "graphics/IPrimitive.h"
#include "graphics/gles/GLVBO.h"

namespace ICSS{
namespace graphics{

	class Mesh2D : public IPrimitive {
		uint8_t m_flags;

		template<typename _Ty>
		using vecSp = std::shared_ptr<std::vector<_Ty>>;
		vecSp<Position> m_Position;
		vecSp<Coordinate> m_Coords;
		vecSp<Color> m_Color;
		vecSp<uint16_t> m_Indices;

		int32_t m_vertCount;
		int32_t m_elemCount;

		bool m_isVisible;

		gles::GLVBO m_vertBuf;
		gles::GLVBO m_idxBuf;
	public:
		enum {
			ATTR_POSITION = 0x01,
			ATTR_COORD = 0x02,
			ATTR_COLOR = 0x04,
			ATTR_INDEX = 0x08, //If this is not specified, GL_TRIANGLE_STRIP will be used
			UPLOADED = 0x10,

		};

		struct ShaderAttributes {
			GLint attr_pos;
			GLint attr_uv;
			GLint attr_color;
		};

		Mesh2D(void);
		Mesh2D(uint8_t attr, GLenum usage);
		Mesh2D(uint16_t vertCount, uint8_t attr, GLenum usage);
		virtual ~Mesh2D(void);

		std::shared_ptr<std::vector<Position>> positions(void);
		std::shared_ptr<std::vector<Coordinate>> coordinates(void);
		std::shared_ptr<std::vector<Color>> colors(void);
		std::shared_ptr<std::vector<uint16_t>> indices(void);

		void unuse(uint8_t attr);

		uint32_t vertCount(void) const { return std::abs(m_vertCount); }
		uint32_t elemCount(void) const { return std::abs(m_elemCount); }
		void setVertCount(uint32_t cnt);
		void setElemCount(uint32_t cnt);

		int32_t positionOffset(void) const;
		int32_t coordOffset(void) const;
		int32_t colorOffset(void) const;
		int32_t indexOffset(void) const;

		uint32_t positionSize(void) const;
		uint32_t coordSize(void) const;
		uint32_t colorSize(void) const;
		uint32_t indexSize(void) const;

		bool visible(void) const { return m_isVisible; }
		void setVisible(bool visible) { m_isVisible = visible; }

		void upload(void);
		bool isUploaded(void) { checkUploaded(); return m_flags & UPLOADED; }

		virtual void draw(DrawEnv *env) override;
		void draw(DrawEnv *env, const gles::GLShaderSet &shader, const ShaderAttributes &attr);

		void bindVertBuf(void);
		void bindIdxBuf(void);

		Mesh2D(const Mesh2D&) = delete;
		Mesh2D& operator=(const Mesh2D&) = delete;
		Mesh2D(Mesh2D&&) = default;
		Mesh2D& operator=(Mesh2D&&) = default;

	private:
		void checkUploaded(void);
		void dirtyOp(void) { this->m_flags &= ~UPLOADED; }
	};

}
}