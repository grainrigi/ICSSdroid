#pragma once

namespace ICSS{
namespace graphics{

	struct Position2D {
		float x;
		float y;
	};
	struct Position3D {
		float x;
		float y;
		float z;
	};
	typedef Position3D Position;
	typedef Position2D Coordinate;

	struct ColorF {
		float r;
		float g;
		float b;
		float a;
	};
	struct ColorDW {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};
	typedef ColorF Color;
}
}