#pragma once
#include "util/basic/Singleton.h"

namespace ICSS{
namespace graphics{

	class GraphicsManager
	{
		//This class is only for Singleton
		friend class ICSS::Singleton<GraphicsManager>;

	public:

	private:
		GraphicsManager(void);
	};

}
}
