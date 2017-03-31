//
// pch.h
// �W���V�X�e���̃C���N���[�h �t�@�C���̃w�b�_�[�B
//
//�v���R���p�C���ς݃w�b�_�[�𐶐����邽�߂Ƀr���h �V�X�e���ɂ���Ďg�p����܂����B
// pch.cpp �͕K�v�Ȃ��Apch.h ���v���W�F�N�g�̈ꕔ�ł��邷�ׂĂ� cpp �t�@�C����
// �����I�Ɋ܂܂�܂��B
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