#pragma once
#include "pch.h"

namespace ICSS{
namespace graphics{
namespace gles{
	class GLES2Handler {
		EGLDisplay display_;

		EGLSurface surface_; //window surface
		EGLConfig config_; //frame buffer configuration
		EGLint screenWidth_;
		EGLint screenHeight_;

		EGLContext context_;
	public:
		GLES2Handler()
			: display_(EGL_NO_DISPLAY),
			surface_(EGL_NO_SURFACE),
			config_(nullptr),
			screenWidth_(), screenHeight_(),
			context_(EGL_NO_CONTEXT)
		{}
		~GLES2Handler()
		{
			destroy();
		}
		GLES2Handler(const GLES2Handler &) = delete;
		GLES2Handler &operator=(const GLES2Handler &) = delete;

		bool create(ANativeWindow *window)
		{
			return initContext(window);
		}
		void destroy()
		{
			disconnectDisplay();
		}
		bool recreate(ANativeWindow *window)
		{
			destroy();
			return create(window);
		}

		//
		// Attach/Detach ANativeWindow
		//

		static const bool preserveEGLContextOnPause = true;
		bool initWindow(ANativeWindow *window)
		{
			LOGI("initWindow");
			if (preserveEGLContextOnPause && hasDisplay() && !hasSurface() && hasContext() && isContextInitialized()) {
				// Resume (Try create window surface by same config)
				LOGI("Trying to resume");
				if (createSurface(window, config_) && makeContextCurrent(window)) {
					LOGI("resume succeeded");
					return true;
				}
				LOGI("resume failed");
			}
			// Recreate all objects.
			return recreate(window);
		}

		void termWindow()
		{
			if (!preserveEGLContextOnPause) {
				destroyContext();
			}
			destroySurface();
		}


		//
		// Presentation
		//

		bool presentFrame(ANativeWindow *window)
		{
			if (!isContextInitialized()) {
				LOGE("Context not initialized");
				return false;
			}

			if (!swap()) {
				recreate(window);
				if (!swap()) {
					LOGE("Swap failed twice");
					return false;
				}
			}
			return true;
		}

	private:
		bool swap()
		{
			if (hasSurface()) {
				LOGI("swap");
				if (!eglSwapBuffers(display_, surface_)) {
					LOGI("eglSwapBuffers failed");
					return false;
				}
			}
			return true;
		}

		virtual void drawFrame() {}

		//
		// Display
		//
	private:
		bool hasDisplay() const { return display_ != EGL_NO_DISPLAY; }

		bool connectDisplay()
		{
			GLint major, minor;

			if (display_ == EGL_NO_DISPLAY) {
				// Get current display.
				const EGLDisplay uninitializedDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
				if (uninitializedDisplay == EGL_NO_DISPLAY) {
					// Do not terminate display.
					LOGE("eglGetDisplay failed");
					return false;
				}
				if (!eglInitialize(uninitializedDisplay, &major, &minor)) {
					LOGE("eglInitialize failed");
					return false;
				}
				display_ = uninitializedDisplay;
			}
			return true;
		}
		void disconnectDisplay()
		{
			destroyContext();
			destroySurface();

			if (display_ != EGL_NO_DISPLAY) {
				LOGI("eglTerminate");
				eglTerminate(display_);
				display_ = EGL_NO_DISPLAY;
			}
		}

		//
		// Config
		//
	private:
		static EGLConfig chooseConfig(EGLDisplay display)
		{
			struct DesiredConfig
			{
				int depth;
				DesiredConfig(int depth) :depth(depth) {}
			};
			const DesiredConfig desiredConfigs[] = { DesiredConfig(24), DesiredConfig(16) };
			const DesiredConfig * const dcEnd = desiredConfigs + sizeof(desiredConfigs) / sizeof(desiredConfigs[0]);
			const DesiredConfig *dc;
			EGLConfig config = nullptr;
			for (dc = desiredConfigs; dc != dcEnd; ++dc) {
				const EGLint desiredAttribs[] = {
					EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, //Request opengl ES2.0
					EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
					EGL_BLUE_SIZE, 8,
					EGL_GREEN_SIZE, 8,
					EGL_RED_SIZE, 8,
					EGL_DEPTH_SIZE, dc->depth,
					EGL_NONE };

				EGLint numConfigs;
				if (!eglChooseConfig(display, desiredAttribs, &config, 1, &numConfigs)) {
					LOGE("eglChooseConfig failed");
					return nullptr;
				}
				if (numConfigs == 1) {
					break;
				}
			}
			if (config == nullptr) {
				LOGE("no EGL config");
				return nullptr;
			}
			LOGI("match config depth=%d", dc->depth);
			return config;
		}


		//
		// Window Surface
		//
	private:
		bool hasSurface() const { return surface_ != EGL_NO_SURFACE; }

		bool createSurface(ANativeWindow *window, EGLConfig desiredConfig = nullptr)
		{
			if (!connectDisplay()) {
				return false;
			}

			if (surface_ == EGL_NO_SURFACE) {
				const EGLConfig config = desiredConfig ? desiredConfig : chooseConfig(display_);
				if (!config) {
					LOGE("chooseConfig failed");
					return false;
				}

				// ANativeWindow_setBuffersGeometry
				EGLint format;
				eglGetConfigAttrib(display_, config, EGL_NATIVE_VISUAL_ID, &format);
				ANativeWindow_setBuffersGeometry(window, 0, 0, format);

				// Create surface.
				surface_ = eglCreateWindowSurface(display_, config, window, nullptr);
				if (surface_ == EGL_NO_SURFACE) {
					LOGE("eglCreateWindowSurface failed. error=%d", eglGetError());
					return false;
				}
				config_ = config;

				eglQuerySurface(display_, surface_, EGL_WIDTH, &screenWidth_);
				eglQuerySurface(display_, surface_, EGL_HEIGHT, &screenHeight_);
				LOGI("screen size %d x %d", screenWidth_, screenHeight_);

				glDrawArraysInstanced(0, 0, 0, 0);
			}
			return true;
		}
		void destroySurface()
		{
			//detachCurrentContext(); // å„ë±ÇÃunloadResourcesÇÃÇΩÇﬂÇ…ç°ÇÕdetachÇµÇ»Ç¢Ç±Ç∆Ç…Ç∑ÇÈÅB

			if (surface_ != EGL_NO_SURFACE) {
				LOGI("eglDestroySurface(window surface)");
				eglDestroySurface(display_, surface_);
				surface_ = EGL_NO_SURFACE;
			}
		}
	public:
		int getScreenWidth() const { return screenWidth_; }
		int getScreenHeight() const { return screenHeight_; }

		//
		// Context
		//
	private:
		bool hasContext() const { return context_ != EGL_NO_CONTEXT; }
		bool createContext(ANativeWindow *window)
		{
			if (!createSurface(window)) { //Needs display_ & config_
				return false;
			}

			if (context_ == EGL_NO_CONTEXT) {
				const EGLint attribs[] = {
					EGL_CONTEXT_CLIENT_VERSION, 2, //Request opengl ES2.0
					EGL_NONE };
				context_ = eglCreateContext(display_, config_, nullptr, attribs);
				if (context_ == EGL_NO_CONTEXT) {
					LOGE("eglCreateContext failed");
					return false;
				}
			}
			return true;
		}
		void destroyContext()
		{
			termContext();

			if (context_ != EGL_NO_CONTEXT) {
				LOGI("eglMakeCurrent null");
				eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
				LOGI("eglDestroyContext");
				eglDestroyContext(display_, context_);
				context_ = EGL_NO_CONTEXT;
			}
		}

		// Current Context

		bool makeContextCurrent(ANativeWindow *window)
		{
			if (!createSurface(window)) {
				return false;
			}
			if (!createContext(window)) {
				return false;
			}

			LOGI("eglMakeCurrent");
			if (!eglMakeCurrent(display_, surface_, surface_, context_)) {
				LOGE("eglMakeCurrent failed");
				return false;
			}
			return true;
		}
		void detachCurrentContext()
		{
			if (display_ != EGL_NO_DISPLAY) {
				LOGI("detachCurrentContext");
				eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
			}
		}
		bool isContextCurrent()
		{
			return context_ != EGL_NO_CONTEXT && eglGetCurrentContext() == context_;
		}

		// Context Initialization

		bool contextInitialized_ = false;
	protected:
		bool isContextInitialized() const { return contextInitialized_; }
	private:
		bool initContext(ANativeWindow *window)
		{
			if (!makeContextCurrent(window)) {
				return false;
			}
			if (!contextInitialized_) {
				if (!loadResources()) {
					return false;
				}
				initializeContextState();
				contextInitialized_ = true;
			}
			LOGI((const char*)glGetString(GL_VERSION));
			return true;
		}
		void termContext()
		{
			if (contextInitialized_) {
				unloadResources();
				contextInitialized_ = false;
			}
		}

		virtual bool loadResources() { return true; }
		virtual void unloadResources() {}
		virtual void initializeContextState() {}

	};
}
}
}