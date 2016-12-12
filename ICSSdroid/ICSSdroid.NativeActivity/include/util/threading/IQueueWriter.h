#pragma once

namespace ICSS {
namespace threading {

	template<typename _Ty>
	class IQueueWriter {
	public:
		virtual bool put(_Ty &buffer) = 0;
	};

}
}