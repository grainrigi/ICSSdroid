#pragma once
#include "pch.h"

#include "util/basic/Singleton.h"
#include "android/Asset.h"

namespace ICSS{
namespace android{

	class AssetManager {
		//This class is only for singleton
		friend class ICSS::Singleton<AssetManager, AAssetManager*>;

		AAssetManager *m_mgr;
	public:
		Asset loadAsset(const std::string &fileName);
	private:
		AssetManager(AAssetManager *mgr);

		//disabled
		AssetManager(const AssetManager&) = delete;
		AssetManager& operator=(const AssetManager&) = delete;
		AssetManager(const AssetManager&&) = delete;
		AssetManager& operator=(const AssetManager&&) = delete;
	};

	typedef ICSS::Singleton<AssetManager, AAssetManager*> AssetManagerSingleton;
}
}