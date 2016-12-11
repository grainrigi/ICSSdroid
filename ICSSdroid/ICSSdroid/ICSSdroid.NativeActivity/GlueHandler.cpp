#include "pch.h"
#include "GlueHandler.h"
#include "InputManager.h"

using namespace ICSS::Frame;

GlueHandler::GlueHandler(android_app *app) : m_glue(app)
{
	app->userData = this;
	app->onAppCmd = GlueHandler::handleCmdStatic;
	app->onInputEvent = GlueHandler::handleInputStatic;
}

GlueHandler::~GlueHandler(void)
{
}

void GlueHandler::run(void)
{
	// event loop
	for (;;) {
		int ident;
		int events;
		android_poll_source* source;

		while ((ident = ALooper_pollAll(-1, NULL, &events, (void**)&source)) >= 0) {
			// Process this event.
			if (ident >= 0 && ident < LOOPER_ID_USER) {
				// Dispatch LOOPER_ID_MAIN, LOOPER_ID_INPUT
				if (source != NULL) {
					source->process(m_glue, source);
				}
			}
			/*
			else if (ident >= LOOPER_ID_USER) {
				processUserLooperId(ident);
			}
			*/

			// Check if we are exiting.
			if (m_glue->destroyRequested != 0) {
				this->Dispose();
				return;
			}
		}
	}
}

void GlueHandler::Dispose(void)
{
	LOGI("destroyApplication");
	this->m_thGraph.Dispose();
}

void GlueHandler::handleCmd(int32_t cmd) {
	switch (cmd) {
		// Window
		case APP_CMD_INIT_WINDOW:
		{
			LOGI("handleCmd(APP_CMD_INIT_WINDOW)");
			// The window is being shown, get it ready.
			break;
		}
		case APP_CMD_TERM_WINDOW:
			LOGI("handleCmd(APP_CMD_TERM_WINDOW)");
			// The window is being hidden or closed, clean it up.
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
			//backupState();
			break;
	}
}

int32_t GlueHandler::handleInput(AInputEvent *event)
{
	Singleton<InputManager>::getInstance().handleInput(event);
	return 0;
}

void GlueHandler::initManagers(void)
{
	Singleton<InputManager>::create();
}
