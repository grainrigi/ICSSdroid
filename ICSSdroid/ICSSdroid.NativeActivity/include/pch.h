﻿//
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

#include <GLES3/gl3.h>
#include <SLES/OpenSLES.h>

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
#include <cassert>
#include "util/basic/std_string_org.h"
#include "util/basic/console_log.h"
#include "graphics/vmath.h"

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





#define FILE_SIG ("(at " + std::string(__FILE__) + ":" + std::to_string(__LINE__) + ")")
#define FILE_FUNC_SIG ("(at " + std::string(__FILE__) + ":" + std::to_string(__LINE__) + " " + std::string(__func__) + "())")