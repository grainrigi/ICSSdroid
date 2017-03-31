/*
(c) 2016,2017 Grain

This file is part of ICSEdit.

ICSEdit is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

ICSEdit is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with ICSEdit.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "graphics/gl/GLInitWindow.h"

using namespace ICSE::graphics::gl;

ICSE::graphics::gl::GLInitWindow::GLInitWindow(void)
	: SDLWindow("GLInit", 1, 1, SDL_WINDOW_OPENGL)
{
	//__asm {int 3};
	OnInit();
}

void ICSE::graphics::gl::GLInitWindow::OnInit(void)
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	m_context = SDL_GL_CreateContext(window());
	GLenum result = glewInit();
	if (result != GLEW_OK)
		THROW(std::runtime_error, "GLEW Init failed.");
		//__asm { int 3 };
#ifdef ICSE_REQUIRE_VSYNC
	if (SDL_GL_SetSwapInterval(1) < 0)
		THROW(std::runtime_error, "Unable to set VSync");
		//__asm { int 3 };
#endif

	SDL_HideWindow(window());
}
