#pragma once

namespace ICSS{
namespace input{
namespace touch{

	struct TouchNotifyParam {
		enum{
			ACTION_DOWN = 1,
			ACTION_UP,
			ACTION_MOVE,
		};

		uint32_t id;
		int32_t action;
		int32_t x;
		int32_t y;
	};

}
}
}