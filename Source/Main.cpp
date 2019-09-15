#include <windows.h>
#include <gdiplus.h>
#include "Exception.h"
#include "Sprite.h"
#include "Animation.h"
#include "Window.h"
#include "Model.h"

#define WND_TITLE_NAME			L"Test Window"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR pCmdLine, _In_ int nCmdShow)
{

	try
	{
		Gdiplus::GdiplusStartupInput	gdiplusStartupInput;
		ULONG_PTR						gdiplusToken;
		Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

		Window* window = new Window(WND_TITLE_NAME);
		Model* model = new Model(window);

		MSG msg;

		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		delete window;
		delete model;

		Gdiplus::GdiplusShutdown(gdiplusToken);

		return static_cast<int>(msg.wParam);
	}
	catch (Exception& exception)
	{
		MessageBox(NULL, std::wstring(L"Exception: " + exception.GetDetails()).c_str(), L"Exception", MB_OK | MB_ICONERROR);
	}
	catch (...)
	{
		MessageBox(NULL, std::wstring(L"Unknown exception!").c_str(), L"Unknown exception", MB_OK | MB_ICONERROR);
	}

	return 0;
}