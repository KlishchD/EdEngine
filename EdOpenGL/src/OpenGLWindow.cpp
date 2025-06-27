#include "EdOpenGL.h"
#include "Window.h"
#include "Helpers/FilesHelper.h"

#include "InputManager.h"

// TODO: If need be add support for multiple windows, currently cutting out existing half solution

void Window::Resize(i32 width, i32 height)
{
	glViewport(0, 0, width, height);
	m_Width = width;
	m_Height = height;

	ED_LOG(Window, info, "Window is resized to {}x{}", width, height) // TODO: Add resize for all screen size framebuffers :)
}