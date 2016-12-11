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
#include <GLES/gl.h>

#include <android/sensor.h>

#include <android/log.h>
#include "android_native_app_glue.h"

#include <string>
#include <sstream>
#include <thread>
#include <functional>
#include <vector>
#include <exception>
#include <memory>
#include <unordered_map>

#define DO_LOG

#ifdef DO_LOG
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "nativeactivitytest", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "nativeactivitytest", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "nativeactivitytest", __VA_ARGS__))
#else
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)
#endif

