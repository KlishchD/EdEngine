#include "Helpers/PlatformHelper.h"
#include <windows.h>
#include <dwmapi.h>
#include <string>

#include "Core/Rendering/EdRendering.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

WNDPROC original_proc;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_NCCALCSIZE:
	{
		// Remove the window's standard sizing border
		if (wParam == TRUE && lParam != NULL)
		{
			NCCALCSIZE_PARAMS* pParams = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
			pParams->rgrc[0].top += 1;
			pParams->rgrc[0].right -= 2;
			pParams->rgrc[0].bottom -= 2;
			pParams->rgrc[0].left += 2;
		}
		return 0;
	}
	case WM_NCPAINT:
	{
		// Prevent the non-client area from being painted
		return 0;
	}
	case WM_NCHITTEST:
	{
		// Expand the hit test area for resizing
		const int borderWidth = 0; // Adjust this value to control the hit test area size

		POINTS mousePos = MAKEPOINTS(lParam);
		POINT clientMousePos = { mousePos.x, mousePos.y };
		ScreenToClient(hWnd, &clientMousePos);

		RECT windowRect;
		GetClientRect(hWnd, &windowRect);

		if (clientMousePos.y >= windowRect.bottom - borderWidth)
		{
			if (clientMousePos.x <= borderWidth)
				return HTBOTTOMLEFT;
			else if (clientMousePos.x >= windowRect.right - borderWidth)
				return HTBOTTOMRIGHT;
			else
				return HTBOTTOM;
		}
		else if (clientMousePos.y <= borderWidth)
		{
			if (clientMousePos.x <= borderWidth)
				return HTTOPLEFT;
			else if (clientMousePos.x >= windowRect.right - borderWidth)
				return HTTOPRIGHT;
			else
				return HTTOP;
		}
		else if (clientMousePos.x <= borderWidth)
		{
			return HTLEFT;
		}
		else if (clientMousePos.x >= windowRect.right - borderWidth)
		{
			return HTRIGHT;
		}

		break;
	}
	}

	return CallWindowProc(original_proc, hWnd, uMsg, wParam, lParam);
}

std::string PlatformHelper::OpenFileWindow(const char* filter, Window& window, const char* title)
{
    OPENFILENAMEA ofn;

    ZeroMemory(&ofn, sizeof(OPENFILENAMEA));

    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*) window.GetNativeWindow());
    
    char filepath[512];
    ofn.lpstrFile = filepath;
    ofn.lpstrFile[0] = '\0';    
    ofn.nMaxFile = sizeof(filepath);

    ofn.nFilterIndex = 1;
    ofn.lpstrFilter = filter;

    ofn.lpstrTitle = title;

    return GetOpenFileNameA(&ofn) == TRUE ? filepath : "";
}

std::string PlatformHelper::SaveFileWindow(const char* filter, Window& window, const char* title)
{
    OPENFILENAMEA ofn;

    ZeroMemory(&ofn, sizeof(OPENFILENAMEA));

    ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)window.GetNativeWindow());

    char filepath[512];
    ofn.lpstrFile = filepath;
    ofn.lpstrFile[0] = '\0';    
    ofn.nMaxFile = sizeof(filepath);

    ofn.nFilterIndex = 1;
    ofn.lpstrFilter = filter;

    ofn.lpstrTitle = title;

    return GetSaveFileNameA(&ofn) == TRUE ? filepath : "";
}

void PlatformHelper::DisableTitleBar(Window& window)
{
	HWND hWnd = glfwGetWin32Window((GLFWwindow*) window.GetNativeWindow());

	LONG_PTR lStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
	lStyle |= WS_THICKFRAME;
	lStyle &= ~WS_CAPTION;
	SetWindowLongPtr(hWnd, GWL_STYLE, lStyle);

	RECT windowRect;
	GetWindowRect(hWnd, &windowRect);
	int width = windowRect.right - windowRect.left;
	int height = windowRect.bottom - windowRect.top;

	original_proc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
	(WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProc));
	SetWindowPos(hWnd, NULL, 0, 0, width, height, SWP_FRAMECHANGED | SWP_NOMOVE);
}
