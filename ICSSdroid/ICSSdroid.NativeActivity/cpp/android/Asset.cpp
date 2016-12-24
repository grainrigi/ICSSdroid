#include "pch.h"
#include "android/Asset.h"

using ICSS::android::Asset;

using ICSS::container::Uint8Array;

Asset::Asset(AAsset * asset)
{
	m_data = Uint8Array(AAsset_getLength(asset));

	AAsset_read(asset, m_data.getPtr(), m_data.getSize());
	AAsset_close(asset);
}
