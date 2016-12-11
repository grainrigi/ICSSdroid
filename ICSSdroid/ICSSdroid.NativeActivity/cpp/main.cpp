#include <android/log.h>
#include <memory>
#include <string>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "android/android_native_app_glue.h"
#include "graphics/GLEnvironment.h"
#include "util/threading/Thread.h"

template<typename FunGetIv, typename FunGetLog>
std::string getGLLogStr(GLuint obj, FunGetIv funGetIv, FunGetLog funGetLog)
{
	GLint len = 0;
	funGetIv(obj, GL_INFO_LOG_LENGTH, &len);
	if (len > 1) {
		std::unique_ptr<char[]> infoLog(new char[len]);
		funGetLog(obj, len, nullptr, infoLog.get());
		return std::string(infoLog.get());
	}
	else {
		return std::string();
	}

	
}
std::string getGLShaderLogInfo(GLuint obj)
{
	return getGLLogStr(obj, glGetShaderiv, glGetShaderInfoLog);
}
std::string getGLProgramLogInfo(GLuint obj)
{
	return getGLLogStr(obj, glGetProgramiv, glGetProgramInfoLog);
}

GLuint loadShader(GLenum type, const char *shaderSrc)
{
	GLuint shader = glCreateShader(type);
	if (shader == 0) {
		return 0;
	}

	glShaderSource(shader, 1, &shaderSrc, nullptr);

	glCompileShader(shader);

	GLint compiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		LOGI("glCompileShader failed.\n Error:(%s)\n", getGLShaderLogInfo(shader).c_str());
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}


/**
* OpenGLES 2.0の初期化からフレームレンダリングまでをカバーしたフレームワークです。
*
* このクラスを継承して必要な仮想関数をオーバーライドしてください。
* リソースの読み込み、解放を行うloadResources()、termResources()やinitializeContextState()、フレームのレンダリングを行うdrawFrame()をオーバーライドしてください。
*
* そしてネイティブウィンドウ(ANativeWindow *)が有効になった時点でinitWindow()を、無効になった時点でtermWindow()を呼び出すようにします。
* ウィンドウへの描画が必要になった時点でpresentFrame()を呼び出してください。
*/




class ThisAppGraphics : public GLEnvironment
{
	GLuint vertexShader_;
	GLuint fragmentShader_;
	GLuint programObject_;
public:
	ThisAppGraphics()
		: GLEnvironment(),
		vertexShader_(0),
		fragmentShader_(0),
		programObject_(0)
	{
	}
private:
	virtual bool loadResources() override
	{
		LOGI("loadResources");

		const char vShaderStr[] =
			"attribute vec4 vPosition;"
			"void main(){"
			"  gl_Position = vPosition;"
			"}";
		const char fShaderStr[] =
			"precision mediump float;"
			"void main(){"
			"  gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);"
			"}";
		vertexShader_ = loadShader(GL_VERTEX_SHADER, vShaderStr);
		fragmentShader_ = loadShader(GL_FRAGMENT_SHADER, fShaderStr);
		programObject_ = glCreateProgram();

		glAttachShader(programObject_, vertexShader_);
		glAttachShader(programObject_, fragmentShader_);
		glBindAttribLocation(programObject_, 0, "vPosition");
		glLinkProgram(programObject_);

		GLint linked = 0;
		glGetProgramiv(programObject_, GL_LINK_STATUS, &linked);
		if (!linked) {
			LOGI("glLinkProgram failed.\n Error:(%s)\n", getGLProgramLogInfo(programObject_).c_str());
			return false;
		}

		return true;
	}
	virtual void unloadResources() override
	{
		LOGI("unloadResources");
		if (programObject_) {
			glDeleteProgram(programObject_);
			programObject_ = 0;
		}
		if (vertexShader_) {
			glDeleteShader(vertexShader_);
			vertexShader_ = 0;
		}
		if (fragmentShader_) {
			glDeleteShader(fragmentShader_);
			fragmentShader_ = 0;
		}
	}
	virtual void initializeContextState() override
	{
		glEnable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glViewport(0, 0, getScreenWidth(), getScreenHeight());
	}

	virtual void drawFrame() override
	{
		LOGI("drawFrame");
		static GLfloat vertices[] = {
			0.0f, 0.5f, 0.0f,
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f
		};
		vertices[1] -= 0.01f;

		glClearColor(0, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(programObject_);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
};

static void testThread(ICSS::threading::Thread *thr) {
	//while(1) {
		if(thr->isTerminating())
			return;
		//usleep(10000);
	//}
}

class ThisApp
{
	android_app *app_;
	ThisAppGraphics gl_;

	struct SavedState
	{
	};
	SavedState state_;
public:
	explicit ThisApp(android_app *app)
		: app_(app)
	{
		app->userData = this;
		app->onAppCmd = handleCmdStatic;
		app->onInputEvent = handleInputStatic;
	}
	ThisApp(const ThisApp &) = delete;
	ThisApp &operator=(const ThisApp &) = delete;
	~ThisApp()
	{
		app_->userData = nullptr;
		app_->onAppCmd = nullptr;
		app_->onInputEvent = nullptr;
	}

private:
	static void handleCmdStatic(android_app *app, int32_t cmd)
	{
		reinterpret_cast<ThisApp *>(app->userData)->handleCmd(cmd);
	}
	static int32_t handleInputStatic(android_app *app, AInputEvent *event)
	{
		return reinterpret_cast<ThisApp *>(app->userData)->handleInput(event);
	}

	void handleCmd(int32_t cmd)
	{
		switch (cmd) {
			// Window
			case APP_CMD_INIT_WINDOW:
			{
				ICSS::threading::Thread th(testThread, nullptr);
				LOGI("handleCmd(APP_CMD_INIT_WINDOW)");
				// The window is being shown, get it ready.
				if (app_->window) {
					gl_.initWindow(app_->window);
					gl_.presentFrame(app_->window);
				}
				timespec spec;
				std::stringstream precis_str;
				clock_getres(CLOCK_MONOTONIC, &spec);
				precis_str << "time_precision:" << spec.tv_nsec;
				LOGI(precis_str.str().c_str());
				break;
			}
			case APP_CMD_TERM_WINDOW:
				LOGI("handleCmd(APP_CMD_TERM_WINDOW)");
				// The window is being hidden or closed, clean it up.
				gl_.termWindow();
				break;
			case APP_CMD_WINDOW_RESIZED: LOGI("handleCmd(APP_CMD_WINDOW_RESIZED)"); break;
			case APP_CMD_WINDOW_REDRAW_NEEDED: LOGI("handleCmd(APP_CMD_WINDOW_REDRAW_NEEDED)"); break;
			case APP_CMD_CONTENT_RECT_CHANGED: LOGI("handleCmd(APP_CMD_CONTENT_RECT_CHANGED)"); break;

			case APP_CMD_GAINED_FOCUS: LOGI("handleCmd(APP_CMD_GAINED_FOCUS)"); break;
			case APP_CMD_LOST_FOCUS: LOGI("handleCmd(APP_CMD_LOST_FOCUS)"); break;

				// Activity State
			case APP_CMD_START: LOGI("handleCmd(APP_CMD_START)"); break;
			case APP_CMD_RESUME: LOGI("handleCmd(APP_CMD_RESUME)"); break;
			case APP_CMD_PAUSE: LOGI("handleCmd(APP_CMD_PAUSE)"); break;
			case APP_CMD_STOP: LOGI("handleCmd(APP_CMD_STOP)"); break;
			case APP_CMD_DESTROY: LOGI("handleCmd(APP_CMD_DESTROY)"); break;

			case APP_CMD_SAVE_STATE:
				LOGI("handleCmd(APP_CMD_SAVE_STATE)");
				backupState();
				break;
		}
	}
	int32_t handleInput(AInputEvent *event)
	{
		LOGI("handleInput(type=%d, deviceId=%d, source=%d)",
			AInputEvent_getType(event),
			AInputEvent_getDeviceId(event),
			AInputEvent_getSource(event));

		switch (AInputEvent_getType(event)) {
			case AINPUT_EVENT_TYPE_KEY:
				LOGI("handleKeyEvent(action=%d, flags=0x%x, keycode=%d, scancode=%d, meta=%d, repeatcount=%d)",
					AKeyEvent_getAction(event),
					AKeyEvent_getFlags(event),
					AKeyEvent_getKeyCode(event),
					AKeyEvent_getScanCode(event),
					AKeyEvent_getMetaState(event),
					AKeyEvent_getRepeatCount(event));
				//return 1;
				break;
			case AINPUT_EVENT_TYPE_MOTION:
				LOGI("handleMotionEvent(action=%d, flags=0x%x, meta=%d, buttonstate=%d, edgeflag=%d, p0x=%f, p0y=%f, size=%f",
					AMotionEvent_getAction(event),
					AMotionEvent_getFlags(event),
					AMotionEvent_getMetaState(event),
					AMotionEvent_getButtonState(event),
					AMotionEvent_getEdgeFlags(event),
					AMotionEvent_getX(event, 0),
					AMotionEvent_getY(event, 0),
					AMotionEvent_getSize(event, 0));
				//size_t AMotionEvent_getPointerCount
				//d AMotionEvent_getPointerId
				//d AMotionEvent_getToolType
				//f AMotionEvent_getRawX
				//...etc(see:<ndkdir>/platforms/android-*/arch-*/usr/include/android/input.h)
				//return 1;
				break;
		}
		return 0;
	}

	void backupState()
	{
		LOGI("backupState");
		app_->savedStateSize = sizeof(SavedState);
		app_->savedState = malloc(sizeof(SavedState));
		*reinterpret_cast<SavedState *>(app_->savedState) = state_;
	}

	void restoreState()
	{
		LOGI("restoreState");
		if (app_->savedState && app_->savedStateSize == sizeof(SavedState)) {
			state_ = *reinterpret_cast<const SavedState *>(app_->savedState);
		}
	}


	int getNextEventDelayTimeMilli()
	{
		// 遅くとも一定時間後までに processInEventLoop() が呼ばれるようにするには、
		// ここでミリ秒を返す。
		// それより早い時間で呼ばれることもある。
		return 17;
	}
	void processInEventLoop()
	{
		// なんらかのイベントが処理された後または、
		// getNextEventDelayTimeMilli()が返した時間経過したときに呼ばれる。
		//gl_.presentFrame(app_->window);
	}

	int looperIdNext_ = LOOPER_ID_USER;
	int getNewLooperId()
	{
		return looperIdNext_++;
	}
	void processUserLooperId(int looperId)
	{
		LOGI("processUserLooperId looperId=%d", looperId);
		// ユーザー定義のLooper ID(LOOPER_ID_USER以上の値)を持つイベントが発生したときに呼ばれる。
	}


	void destroyApplication()
	{
		LOGI("destroyApplication");
	}

public:
	void run()
	{
		// Restore app state
		restoreState();

		// event loop
		for (;;) {
			int ident;
			int events;
			android_poll_source* source;

			while ((ident = ALooper_pollAll(getNextEventDelayTimeMilli(), NULL, &events, (void**)&source)) >= 0) {
				// Process this event.
				if (ident >= 0 && ident < LOOPER_ID_USER) {
					// Dispatch LOOPER_ID_MAIN, LOOPER_ID_INPUT
					if (source != NULL) {
						source->process(app_, source);
					}
				}
				else if (ident >= LOOPER_ID_USER) {
					processUserLooperId(ident);
				}

				// Check if we are exiting.
				if (app_->destroyRequested != 0) {
					destroyApplication();
					return;
				}
			}

			processInEventLoop();
		}
	}
};

void android_main(android_app *state)
{

	ThisApp thisApp(state);
	thisApp.run();
}