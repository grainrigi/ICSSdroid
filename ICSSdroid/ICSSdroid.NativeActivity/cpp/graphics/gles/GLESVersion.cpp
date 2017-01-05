#include "graphics/gles/GLESVersion.h"

using ICSS::graphics::gles::GLESVersion;

int GLESVersion::getGLESVersion(void)
{
	return m_version;
}

void GLESVersion::reportGLESVersion(const char * verstr)
{
	int32_t major = -1, minor = -1;

	//Check 2.x/3.x
	if(sscanf(verstr ,"OpenGL ES %d.%d", &major, &minor) != 2)
	{
		//Check 1.x
		if(sscanf(verstr, "OpenGL ES-C%c 1.%d", reinterpret_cast<char*>(&major), &minor) != 2)
			return;
		major = 1;
	}

	m_version = major * 10 + minor;
}

void GLESVersion::updateWindow(ANativeWindow * window)
{
	this->checkAndSetVersion();
}

GLESVersion::GLESVersion(ANativeWindow *window)
	: m_window(window),
	m_display(EGL_NO_DISPLAY),
	m_config(nullptr),
	m_surface(EGL_NO_SURFACE),
	m_context(EGL_NO_CONTEXT)
{
	this->updateWindow(window);
}

void GLESVersion::checkAndSetVersion(void)
{
	if(!this->createTempContext())
		return;
	reportGLESVersion((const char*)glGetString(GL_VERSION));
	this->destroyTempContext();
}

bool GLESVersion::createTempContext(void)
{
	EGLDisplay display;
	EGLConfig config;
	EGLint numConfigs;
	EGLSurface surface;
	EGLContext context;

	const EGLint desiredAttribs[] = {
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, //Request opengl ES2.0
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_DEPTH_SIZE, 16,
		EGL_NONE };
	const EGLint contextattribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2, //Request opengl ES2.0
		EGL_NONE };
	
	//Init display
	display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if(display == EGL_NO_DISPLAY)
		return false;
	if(!eglInitialize(display, 0, 0))
		return false;
	m_display = display;

	//Choose config
	if(!eglChooseConfig(display, desiredAttribs, &config, 1, &numConfigs))
		return false;
	if(numConfigs != 1 || config == nullptr)
		return false;
	m_config = config;

	//Create surface
	surface = eglCreateWindowSurface(display, config, m_window, nullptr);
	if(surface == EGL_NO_SURFACE)
		return false;
	m_surface = surface;

	//Create context
	context = eglCreateContext(display, config, nullptr, contextattribs);
	if(context == EGL_NO_CONTEXT)
		return false;
	m_context = context;

	//Set current context
	eglMakeCurrent(display, surface, surface, context);

	return true;
}

bool GLESVersion::destroyTempContext(void)
{
	EGLDisplay display = m_display;

	eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	
	if(m_context != EGL_NO_CONTEXT)
	{
		eglDestroyContext(display, m_context);
		m_context = EGL_NO_CONTEXT;
	}

	if(m_surface != EGL_NO_SURFACE)
	{
		eglDestroySurface(display, m_surface);
		m_surface = EGL_NO_SURFACE;
	}

	m_config = nullptr;
	//eglTerminate(display);
	m_display = EGL_NO_DISPLAY;

	return true;
}
