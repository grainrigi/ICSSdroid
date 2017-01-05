#pragma once
#include "pch.h"
#include "util/basic/Singleton.h"

namespace ICSS{
namespace graphics{
namespace gles{	

	/*  *** WARNING!!! ***
		Make sure that you have NOT INITIALIZED the display before you init this class.
		This class will REINITIALIZE THE DISPLAY and may DESTROY THE EXISTING CONTEXT.
	*/

	class GLESVersion
	{
		//This class is only for Singleton
		friend ICSS::Singleton<GLESVersion, ANativeWindow*>;

		uint32_t m_version = 0;
		ANativeWindow *m_window;

		EGLDisplay m_display;
		EGLConfig m_config;
		EGLSurface m_surface;
		EGLContext m_context;

	public:
		enum {
			GL_ES_1_0 = 10,
			GL_ES_1_1 = 11,
			GL_ES_2_0 = 20,
			GL_ES_3_0 = 30,
			GL_ES_3_1 = 31,
			GL_ES_3_2 = 32,
		};

		int getGLESVersion(void);

	private:
		//for friend
		void reportGLESVersion(const char *verstr);
		void updateWindow(ANativeWindow *window);
	private:
		GLESVersion(ANativeWindow *window);

		//Version Check
		void checkAndSetVersion(void);
		bool createTempContext(void);
		bool destroyTempContext(void);

		GLESVersion(const GLESVersion&) = delete;
		GLESVersion &operator=(const GLESVersion&) = delete;
		GLESVersion(const GLESVersion&&) = delete;
		GLESVersion &operator=(const GLESVersion&&) = delete;
	};

	typedef ICSS::Singleton<GLESVersion, ANativeWindow*> GLESVersionSingleton;
}
}
}