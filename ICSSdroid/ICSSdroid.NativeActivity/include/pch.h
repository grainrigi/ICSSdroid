//
// pch.h
// 標準システムのインクルード ファイルのヘッダー。
//
//プリコンパイル済みヘッダーを生成するためにビルド システムによって使用されました。
// pch.cpp は必要なく、pch.h がプロジェクトの一部であるすべての cpp ファイルに
// 自動的に含まれます。
//



#include <jni.h>
#include <errno.h>

#include <string.h>
#include <unistd.h>
#include <sys/resource.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <android/sensor.h>

#include <android/log.h>
#include "android/android_native_app_glue.h"

#include <string>
#include <sstream>
#include <fstream>
#include <thread>
#include <functional>
#include <vector>
#include <exception>
#include <memory>
#include <unordered_map>
#include <map>
#include <atomic>
#include <mutex>
#include <algorithm>

#define DO_LOG

#ifdef DO_LOG
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "icssdroid", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "icssdroid", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "icssdroid", __VA_ARGS__))
#else
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)
#endif

