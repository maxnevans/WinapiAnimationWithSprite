#pragma once
#include <stdint.h>
#include <string>
#include <map>
#include <windows.h>

class Window
{

public:
	typedef LRESULT(*ActionHandler)(HWND, UINT, WPARAM, LPARAM, void*);

private:
	class WindowRegisterClass
	{
	public:
		WindowRegisterClass();
	};

	struct ActionHandlerAndUserObject
	{
		void* objectPointer;
		ActionHandler handler;
	};

private:
	static LRESULT WINAPI WndStartupProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI WndProxyProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static LRESULT HandlerOnSizing(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, Window* self);

	void CalculateWindowProperties();

protected:
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	Window(std::wstring windowName);
	void Create();
	void SetWindowName(std::wstring windowName);
	std::wstring GetWindowName();
	void SetActionHandler(UINT uMsg, ActionHandler handler, void* object);
	int32_t GetClientWidth();
	int32_t GetClientHeight();

private:
	static constexpr const wchar_t* WND_CLASS_NAME = L"MainWindow";
	static constexpr const int32_t DEFAULT_WINDOW_WIDTH = 1280;
	static constexpr const int32_t DEFAULT_WINDOW_HEIGHT = 720;
	static constexpr const bool REDRAW_WINDOW_CONTENT_WHEN_SIZING = true;
	static constexpr const bool REDRAW_WINDOW_CONTENT_WHEN_DRAGGING = true;
	static WindowRegisterClass wndRegClass;

	std::map<UINT, ActionHandlerAndUserObject*> handlers;
	std::wstring windowName;
	HWND hWnd;
	HINSTANCE hInst;

	int32_t clientWidth, clientHeight;
	int32_t windowWidth, windowHeight;
};

