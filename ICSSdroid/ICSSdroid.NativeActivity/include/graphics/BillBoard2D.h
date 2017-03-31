#pragma once
#include "pch.h"
#include "graphics/Mesh2D.h"

namespace ICSS{
namespace graphics{

	//a billboard supposed to contain pixel-based width, height and position
	class BillBoard2D : public Mesh2D {

	public:

	private:
		void initMesh(void);
	};

}
}
