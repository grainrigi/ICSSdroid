#include "android/AssetManager.h"

using ICSS::android::Asset;
using ICSS::android::AssetManager;

Asset AssetManager::loadAsset(const std::string & fileName)
{
	return Asset(AAssetManager_open(m_mgr, fileName.c_str(), AASSET_MODE_RANDOM));
}

AssetManager::AssetManager(AAssetManager * mgr)
	: m_mgr(mgr)
{
}
