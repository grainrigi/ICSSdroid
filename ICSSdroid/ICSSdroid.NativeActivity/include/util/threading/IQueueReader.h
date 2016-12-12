#pragma once

namespace ICSS {
namespace threading {

	template<typename _Ty>
	class IQueueReader {
	public:
		virtual bool get(_Ty *buffer) = 0;
	};

}
}