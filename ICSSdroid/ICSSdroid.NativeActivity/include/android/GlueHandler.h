#pragma once
#include "pch.h"
#include "util/threading/Thread.h"
#include "util/basic/IDisposable.h"

namespace ICSS {
	namespace android {
		class GlueHandler : public IDisposable {
			android_app *m_glue;
			threading::Thread m_thGraph;

		public:
			GlueHandler(android_app *app);
			~GlueHandler(void);
	
			void run(void);

			//IDisposable
			virtual void Dispose(void) override;
		private:
			static void handleCmdStatic(android_app *app, int32_t cmd)
			{
				reinterpret_cast<GlueHandler *>(app->userData)->handleCmd(cmd);
			}
			static int32_t handleInputStatic(android_app *app, AInputEvent *event)
			{
				return reinterpret_cast<GlueHandler *>(app->userData)->handleInput(event);
			}

			void handleCmd(int32_t cmd);
			int32_t handleInput(AInputEvent *event);

			void onInitWnd(void);
			void initManagers(void);
		};
	}
}