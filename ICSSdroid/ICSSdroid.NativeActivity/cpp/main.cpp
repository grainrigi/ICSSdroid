#include "pch.h"

#include "util/file/stb_image.h"

#include "android/android_native_app_glue.h"
#include "android/AssetManager.h"
#include "graphics/GLEnvironment.h"
#include "graphics/gles/GLESVersion.h"
#include "graphics/gles/GLShaderSet.h"
#include "graphics/gles/GLVBO.h"
#include "graphics/gles/GLTexture.h"
#include "graphics/Mesh2D.h"
#include "graphics/Mesh2DRenderer.h"
#include "graphics/BillBoard2D.h"
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
using ICSS::graphics::gles::GLTexture;
using ICSS::graphics::Mesh2D;
using ICSS::graphics::Mesh2DRenderer;
using ICSS::graphics::BillBoard2D;
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

void createVpTrans(GLfloat *ary, float width, float height)
{
	ary[0] = 2.0f / width;
	ary[1] = 2.0f / height;
	ary[2] = 1.0f;
	ary[3] = 1.0f;
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

void SoundPlay(void)
{

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
	GLint unif_phase_;
	GLTexture texture_;
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
			-300.0f, 300.0f, 0.0f,
			-300.0f, -300.0f, 0.0f,
			300.0f, 300.0f, 0.0f,
			300.0f, -300.0f, 0.0f
		};
		constexpr GLfloat coords[]{
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 0.0f,
			1.0f, 1.0f
		};
		
		constexpr GLfloat colors[]{
			PI / 2.0f, 0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, PI / 2.0f, 1.0f,
			0.0f, PI / 2.0f, 0.0f, 1.0f,
			PI / 2.0f, PI / 2.0f, 0.0f, 1.0f,
		};

		GLfloat vpTrans[4];
		GLuint unif_vptrans;
		GLuint unif_texture;

		std::string vShader{
			"uniform mediump float unif_phase;"
			"uniform mediump vec4 unif_vptrans;"
			"attribute mediump vec4 attr_pos; "
			"attribute mediump vec2 attr_uv;"
			"varying mediump vec2 vary_uv;"
			"void main(){"
			"  gl_Position = attr_pos * unif_vptrans;"
			"  vary_uv = attr_uv;"
			"  vary_uv.x = vary_uv.x + sin(unif_phase);"
			/*
			"  vary_color = (sin(attr_color + unif_phase) + 1.0) * 0.5;"
			"  vary_color.a = 1.0;"
			*/
			"}"
		};
		std::string fShader{
			"uniform sampler2D unif_texture;"
			"varying mediump vec2 vary_uv;"
			"precision mediump float;"
			"void main(){"
			"  gl_FragColor = texture2D(unif_texture, vary_uv);"
			"}"
		};

		try {
			shader_ = GLShaderSet::createFromString(vShader, fShader, 
			{
				{ 0, "attr_pos" },
				{ 1, "attr_color" },
			});
			unif_phase_ = shader_.getUniformLocation("unif_phase");
			unif_vptrans = shader_.getUniformLocation("unif_vptrans");
			unif_texture = shader_.getUniformLocation("unif_texture");
			shader_.use();
			createVpTrans(vpTrans, (float)getScreenWidth(), (float)getScreenHeight());
			glUniform4fv(unif_vptrans, 1, vpTrans);
			glUniform1i(unif_texture, 0);
		}
		catch(std::runtime_error ex) {
			LOGE("%s", ex.what());
			throw;
		}
 
		//create texture
		Asset texpng = AssetManagerSingleton::getInstance().loadAsset("ninapri.png");
		ImageFile img = ImageFile::loadFromFile(texpng, ImageFile::FORMAT_RGBA);
		texture_.init();
		texture_.uploadImage(img);
		glEnable(GL_TEXTURE_2D);

		vertices[0] = vertices[3] = -(vertices[6] = vertices[9] = (float)538 / 2.0f);
		vertices[4] = vertices[10] = -(vertices[1] = vertices[7] = (float)640 / 2.0f);

		//create mesh
		mesh_ = Mesh2D(4 ,Mesh2D::ATTR_POSITION | Mesh2D::ATTR_COORD, GL_STATIC_DRAW);
		memcpy(&(*mesh_.positions())[0], vertices, sizeof(vertices));
		memcpy(&(*mesh_.coordinates())[0], coords, sizeof(coords));
		mesh_.upload();

		BillBoard2D bb;

		GLuint fbo;
		glGenFramebuffers(1, &fbo);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		GLenum db = GL_BACK;
		glDrawBuffers(1, &db);
		glBlitFramebuffer(0, 0, 512, 512, 0, 0, 512, 512, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		return true;
	}
	virtual void unloadResources() override
	{
		LOGI("unloadResources");
		glBindTexture(GL_TEXTURE_2D, 0);
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
		Mesh2D::ShaderAttributes attr;
		static float phase = 0.0f;

		phase += 0.1f;

		glClearColor(0, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		texture_.bind();
		env.setShader(shader_);
		glUniform1f(unif_phase_, phase);

		attr.attr_pos = 0;
		attr.attr_uv = 1;
		attr.attr_color = -1;

		mesh_.draw(&env, shader_, attr);
	}
};

static constexpr int AUDIO_DATA_STORAGE_SIZE = 4096;
static constexpr int AUDIO_DATA_BUFFER_COUNT = 8;
static constexpr int AUDIO_DATA_BUFFER_SIZE = AUDIO_DATA_STORAGE_SIZE / AUDIO_DATA_BUFFER_COUNT;

/* Structure for passing information to callback function */
typedef struct CallbackCntxt_ {
	SLPlayItf playItf;
	SLint16* pDataBase; // Base adress of local audio data storage
	SLint16* pData; // Current adress of local audio data storage
	SLuint32 size;
} CallbackCntxt;

void CheckErr(SLresult res)
{
	if (res != SL_RESULT_SUCCESS)
		throw std::runtime_error("SL_UNSUCCESS");
}

void BufferQueueCallback(
	SLBufferQueueItf queueItf,
	void *pContext
)
{
	SLresult res;
	CallbackCntxt *pCntxt = (CallbackCntxt*)pContext;
	if (pCntxt->pData < (pCntxt->pDataBase + pCntxt->size))
	{
		res = (*queueItf)->Enqueue(queueItf, (void*)pCntxt->pData,
			2 * AUDIO_DATA_BUFFER_SIZE);
		if (res != SL_RESULT_SUCCESS)
			throw std::runtime_error("SL_UNSCCESS");
		pCntxt->pData += AUDIO_DATA_BUFFER_SIZE;
	}
}

static void testThread(ICSS::threading::Thread *thr) {
	SLObjectItf sl;
	SLresult res;
	SLEngineOption EngineOption[] = {
		(SLuint32)SL_ENGINEOPTION_THREADSAFE,
		(SLuint32)SL_BOOLEAN_TRUE };
	res = slCreateEngine(&sl, 1, EngineOption, 0, NULL, NULL);
	CheckErr(res);
	/* Realizing the SL Engine in synchronous mode. */
	res = (*sl)->Realize(sl, SL_BOOLEAN_FALSE); CheckErr(res);

	auto wav = ICSS::android::AssetManagerSingleton::getInstance().loadAsset("wave.wav");

	int size = wav.getSize();

	SLEngineItf EngineItf;
	SLint32 numOutputs = 0;
	SLuint32 deviceID = 0;
	
	SLDataSource audioSource;
	SLDataLocator_BufferQueue bufferQueue;
	SLDataFormat_PCM pcm;
	SLDataSink audioSink;
	SLDataLocator_OutputMix locator_outputmix;
	SLObjectItf player;
	SLPlayItf playItf;
	SLBufferQueueItf bufferQueueItf;
	SLBufferQueueState state;
	SLObjectItf OutputMix;
	//SLVolumeItf volumeItf;
	int i;

	SLboolean required[3];
	SLInterfaceID iidArray[3];
	/* Callback context for the buffer queue callback function */
	CallbackCntxt cntxt;
	/* Get the SL Engine Interface which is implicit */
	res = (*sl)->GetInterface(sl, SL_IID_ENGINE, (void*)&EngineItf);
	

	/* Initialize arrays required[] and iidArray[] */

	for (i = 0; i<3; i++)
	{
		required[i] = SL_BOOLEAN_FALSE;
		iidArray[i] = SL_IID_NULL;
	}
	// Set arrays required[] and iidArray[] for VOLUME interface
	required[0] = SL_BOOLEAN_TRUE;
	iidArray[0] = SL_IID_VOLUME;
	// Create Output Mix object to be used by player
	res = (*EngineItf)->CreateOutputMix(EngineItf, &OutputMix, 0,
		NULL, NULL); //CheckErr(res);
	// Realizing the Output Mix object in synchronous mode.
	res = (*OutputMix)->Realize(OutputMix, SL_BOOLEAN_FALSE);
	CheckErr(res);
	//res = (*OutputMix)->GetInterface(OutputMix, SL_IID_VOLUME,
		//(void*)&volumeItf); CheckErr(res);
	/* Setup the data source structure for the buffer queue */
	bufferQueue.locatorType = SL_DATALOCATOR_BUFFERQUEUE;
	bufferQueue.numBuffers = 4; /* Four buffers in our buffer queue */

								/* Setup the format of the content in the buffer queue */
	pcm.formatType = SL_DATAFORMAT_PCM;
	pcm.numChannels = 2;
	pcm.samplesPerSec = SL_SAMPLINGRATE_44_1;
	pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
	pcm.containerSize = 16;
	pcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
	pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;
	audioSource.pFormat = (void *)&pcm;
	audioSource.pLocator = (void *)&bufferQueue;
	/* Setup the data sink structure */
	locator_outputmix.locatorType = SL_DATALOCATOR_OUTPUTMIX;
	locator_outputmix.outputMix = OutputMix;
	audioSink.pLocator = (void *)&locator_outputmix;
	audioSink.pFormat = NULL;
	/* Initialize the context for Buffer queue callbacks */
	cntxt.pDataBase = (SLint16*)wav.getData();
	cntxt.pData = cntxt.pDataBase;
	cntxt.size = size;
	/* Set arrays required[] and iidArray[] for SEEK interface
	(PlayItf is implicit) */
	required[0] = SL_BOOLEAN_TRUE;
	iidArray[0] = SL_IID_BUFFERQUEUE;
	/* Create the music player */
	res = (*EngineItf)->CreateAudioPlayer(EngineItf, &player,
		&audioSource, &audioSink, 1, iidArray, required); CheckErr(res);
	/* Realizing the player in synchronous mode. */
	res = (*player)->Realize(player, SL_BOOLEAN_FALSE); CheckErr(res);
	/* Get seek and play interfaces */
	res = (*player)->GetInterface(player, SL_IID_PLAY, (void*)&playItf);
	CheckErr(res);
	res = (*player)->GetInterface(player, SL_IID_BUFFERQUEUE,
		(void*)&bufferQueueItf); CheckErr(res);
	/* Setup to receive buffer queue event callbacks */
	res = (*bufferQueueItf)->RegisterCallback(bufferQueueItf,
		BufferQueueCallback, &cntxt); CheckErr(res);
	/* Before we start set volume to -3dB (-300mB) */
	//res = (*volumeItf)->SetVolumeLevel(volumeItf, -300); CheckErr(res);
	/* Enqueue a few buffers to get the ball rolling */
	res = (*bufferQueueItf)->Enqueue(bufferQueueItf, cntxt.pData,
		2 * AUDIO_DATA_BUFFER_SIZE); /* Size given in bytes. */
	CheckErr(res);
	cntxt.pData += AUDIO_DATA_BUFFER_SIZE;
	res = (*bufferQueueItf)->Enqueue(bufferQueueItf, cntxt.pData,
		2 * AUDIO_DATA_BUFFER_SIZE); /* Size given in bytes. */
	CheckErr(res);
	cntxt.pData += AUDIO_DATA_BUFFER_SIZE;
	res = (*bufferQueueItf)->Enqueue(bufferQueueItf, cntxt.pData,
		2 * AUDIO_DATA_BUFFER_SIZE); /* Size given in bytes. */
	CheckErr(res);
	cntxt.pData += AUDIO_DATA_BUFFER_SIZE;
	/* Play the PCM samples using a buffer queue */
	res = (*playItf)->SetPlayState(playItf, SL_PLAYSTATE_PLAYING);
	CheckErr(res);
	/* Wait until the PCM data is done playing, the buffer queue callback
	will continue to queue buffers until the entire PCM data has been
	played. This is indicated by waiting for the count member of the
	SLBufferQueueState to go to zero.
	*/
	res = (*bufferQueueItf)->GetState(bufferQueueItf, &state);
	CheckErr(res);

	while (state.count)
	{
		if (thr->isTerminating())
			break;
		(*bufferQueueItf)->GetState(bufferQueueItf, &state);
	}
	/* Make sure player is stopped */
	res = (*playItf)->SetPlayState(playItf, SL_PLAYSTATE_STOPPED);
	CheckErr(res);
	/* Destroy the player */
	(*player)->Destroy(player);
	/* Destroy Output Mix object */
	(*OutputMix)->Destroy(OutputMix);
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
				//ICSS::threading::Thread th(testThread, nullptr);
				LOGI("handleCmd(APP_CMD_INIT_WINDOW)");
				GLESVersionSingleton::create(app_->window);
				LOGI("OpenGLES Version : %d", GLESVersionSingleton::getInstance().getGLESVersion());
				// The window is being shown, get it ready.
				if (app_->window) {
					gl_.initWindow(app_->window);
					Singleton<Mesh2DRenderer>::create();
					gl_.presentFrame(app_->window);
				}
				
				//Get Context Class descriptor
				JNIEnv *env;
				app_->activity->vm->AttachCurrentThread(&env, NULL);
				jclass contextClass = env->FindClass("android/content/Context");
				jmethodID getExternalFilesDirMethodId = env->GetMethodID(contextClass, "getExternalFilesDir", "(Ljava/lang/String;)Ljava/io/File;");
				jstring getExternalType = env->NewStringUTF("");
				jobject fileObject = env->CallObjectMethod(app_->activity->clazz, getExternalFilesDirMethodId, getExternalType);
				jclass fileClass = env->FindClass("java/io/File");
				jmethodID absolutePathMethodId = env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
				jstring stringObject = (jstring)env->CallObjectMethod(fileObject, absolutePathMethodId);
				const char *str1 = env->GetStringUTFChars(stringObject, 0);
				std::string str(str1);
				env->ReleaseStringUTFChars(stringObject, str1);

				app_->activity->vm->DetachCurrentThread();
				
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