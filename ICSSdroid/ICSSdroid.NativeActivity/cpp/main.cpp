#include "pch.h"

#include "util/file/stb_image.h"

#include "android/android_native_app_glue.h"
#include "android/AssetManager.h"
#include "graphics/GLEnvironment.h"
#include "graphics/gles/GLESVersion.h"
#include "graphics/gles/GLShaderSet.h"
#include "graphics/gles/GLVBO.h"
#include "graphics/Mesh2D.h"
#include "graphics/Mesh2DRenderer.h"
#include "util/threading/Thread.h"
#include "input/InputManager.h"
#include "util/threading/LockFreeQueue8.h"
#include "util/file/ImageFile.h"

using ICSS::Singleton;
using ICSS::threading::LockFreeQueue8;
using ICSS::android::AssetManagerSingleton;
using ICSS::android::Asset;
using ICSS::graphics::gles::GLESVersionSingleton;
using ICSS::graphics::gles::GLShaderSet;
using ICSS::graphics::gles::GLVBO;
using ICSS::graphics::Mesh2D;
using ICSS::graphics::Mesh2DRenderer;
using ICSS::graphics::DrawEnv;
using ICSS::input::InputManager;
using ICSS::input::touch::TouchNotifyParam;
using ICSS::input::touch::TouchSensor;
using ICSS::file::ImageFile;

constexpr float PI = 3.1415926535f;

template<typename FunGetIv, typename FunGetLog>
std::string getGLLogStr(GLuint obj, FunGetIv funGetIv, FunGetLog funGetLog);
std::string getGLShaderLogInfo(GLuint obj);
std::string getGLProgramLogInfo(GLuint obj);

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
	GLint unif_tex_;
	GLint unif_phase_;
	GLuint texture_;
	GLShaderSet shader_;
	DrawEnv env_;
	Mesh2D mesh_;

public:
	ThisAppGraphics()
		: GLEnvironment()
	{
	}
private:
	virtual bool loadResources() override
	{
		LOGI("loadResources");

		GLfloat vertices[]{
			-0.5f, -0.5f, 0.0f,
			-0.5f, 0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.5f, 0.5f, 0.0f
		};
		GLfloat coords[]{
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 0.0f,
			1.0f, 1.0f
		};
		
		GLfloat colors[]{
			PI / 2.0f, 0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, PI / 2.0f, 1.0f,
			0.0f, PI / 2.0f, 0.0f, 1.0f,
			PI / 2.0f, PI / 2.0f, 0.0f, 1.0f,
		};

		std::string vShader{
			"uniform mediump float unif_phase;"
			"attribute mediump vec4 attr_pos; "
			"attribute mediump vec4 attr_color;"
			"varying mediump vec4 vary_color;"
			"void main(){"
			"  gl_Position = attr_pos;"
			"  vary_color = (sin(attr_color + unif_phase) + 1.0) * 0.5;"
			"  vary_color.a = 1.0;"
			"}"
		};
		std::string fShader{
			"varying mediump vec4 vary_color;"
			"precision mediump float;"
			"void main(){"
			"  gl_FragColor = vary_color;"
			"}"
		};

		try {
			shader_ = GLShaderSet::createFromString(vShader, fShader, 
			{
				{ 0, "attr_pos" },
				{ 1, "attr_color" },
			});
			unif_phase_ = glGetUniformLocation(shader_.program(), "unif_phase");
		}
		catch(std::runtime_error ex) {
			LOGE("%s", ex.what());
			return false;
		}
 
		//create texture
		Asset texpng = AssetManagerSingleton::getInstance().loadAsset("ninapri.png");
		ImageFile img = ImageFile::loadFromFile(texpng, ImageFile::FORMAT_RGBA);
		glGenTextures(1, &texture_);
		glBindTexture(GL_TEXTURE_2D, texture_);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getX(), img.getY(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixels());
		GLenum err = glGetError();
		if(err != GL_NO_ERROR)
			LOGE("glTexImage2D failed.Error:%d", err);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // 拡大時近傍
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // 縮小時近傍
		glEnable(GL_TEXTURE_2D);

		mesh_ = Mesh2D(4 ,Mesh2D::ATTR_POSITION | Mesh2D::ATTR_COLOR, GL_STATIC_DRAW);
		memcpy(&(*mesh_.positions())[0], vertices, sizeof(vertices));
		memcpy(&(*mesh_.colors())[0], colors, sizeof(colors));
		mesh_.upload();

		return true;
	}
	virtual void unloadResources() override
	{
		LOGI("unloadResources");
		glBindTexture(GL_TEXTURE_2D, 0);

		if(texture_) {
			glDeleteTextures(1, &texture_);
		}
	}
	virtual void initializeContextState() override
	{
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glViewport(0, 0, getScreenWidth(), getScreenHeight());
	}

	virtual void drawFrame() override
	{
		//LOGI("drawFrame");
		DrawEnv env;
		Mesh2D::ShaderAttributes attr { 0, -1, 1 };
		static float phase = 0.0f;

		phase += 0.1f;

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glBindTexture(GL_TEXTURE_2D, texture_);
		glUseProgram(shader_.program());
		glUniform1f(unif_phase_, phase);

		mesh_.draw(&env, shader_, attr);
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
	LockFreeQueue8<TouchNotifyParam> iqueue_;
	uint32_t iqid_;
	TouchSensor::Data tsdata_ = { 100, 100, 500, 500 };
	bool working = false;


	struct SavedState
	{
	};
	SavedState state_;
public:
	explicit ThisApp(android_app *app)
		: app_(app),
		iqueue_(64)
	{
		app->userData = this;
		app->onAppCmd = handleCmdStatic;
		app->onInputEvent = handleInputStatic;
		tsdata_.queue = &iqueue_;
		Singleton<InputManager>::create();
		AssetManagerSingleton::create(app->activity->assetManager);
		iqid_ = Singleton<InputManager>::getInstance().registerFingerListner(&iqueue_);
	}
	ThisApp(const ThisApp &) = delete;
	ThisApp &operator=(const ThisApp &) = delete;
	~ThisApp()
	{
		app_->userData = nullptr;
		app_->onAppCmd = nullptr;
		app_->onInputEvent = nullptr;
		Singleton<Mesh2DRenderer>::destroy();
		GLESVersionSingleton::destroy();
		AssetManagerSingleton::destroy();
		Singleton<InputManager>::getInstance().unregisterFingerListner(iqid_);
		Singleton<InputManager>::destroy();
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
				GLESVersionSingleton::create(app_->window);
				LOGI("OpenGLES Version : %d", GLESVersionSingleton::getInstance().getGLESVersion());
				// The window is being shown, get it ready.
				if (app_->window) {
					gl_.initWindow(app_->window);
					Singleton<Mesh2DRenderer>::create();
					gl_.presentFrame(app_->window);
				}

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
			case APP_CMD_START: {LOGI("handleCmd(APP_CMD_START)");working = true; break;}
			case APP_CMD_RESUME: LOGI("handleCmd(APP_CMD_RESUME)"); working = true; break;
			case APP_CMD_PAUSE: {LOGI("handleCmd(APP_CMD_PAUSE)"); working = false; break;}
			case APP_CMD_STOP: LOGI("handleCmd(APP_CMD_STOP)"); working = false; break;
			case APP_CMD_DESTROY: LOGI("handleCmd(APP_CMD_DESTROY)"); working = false; break;

			case APP_CMD_SAVE_STATE:
				LOGI("handleCmd(APP_CMD_SAVE_STATE)");
				backupState();
				break;
		}
	}
	int32_t handleInput(AInputEvent *event)
	{
		/*
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
				break;
		}
		*/
		Singleton<InputManager>::getInstance().handleInput(event);
		TouchNotifyParam param;
		while(this->iqueue_.get(&param)) {
			LOGI("handleMotionEvent(action=%d, fingerid=%d, x=%d, y=%d)",
				param.action,
				param.id,
				param.x,
				param.y);
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
		if(working)
			gl_.presentFrame(app_->window);
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
	try{
	ThisApp thisApp(state);
	thisApp.run();

	}
	catch(std::runtime_error ex) {
		LOGE("%s", ex.what());
		return;
	}
}