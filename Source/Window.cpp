#include "Window.h"
#include "Exception.h"

Window::WindowRegisterClass::WindowRegisterClass()
{
	WNDCLASSEX wcex = { 0 };

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.style = Window::REDRAW_WINDOW_CONTENT_WHEN_SIZING ? CS_HREDRAW | CS_VREDRAW : NULL;
	wcex.lpfnWndProc = Window::WndStartupProc;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wcex.lpszClassName = Window::WND_CLASS_NAME;

	if (RegisterClassEx(&wcex) == 0)
		throw Exception(L"Failed to register window class!");
}

Window::WindowRegisterClass Window::wndRegClass;

LRESULT WINAPI Window::WndStartupProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg != WM_NCCREATE)
		return DefWindowProc(hWnd, uMsg, wParam, lParam);

	if (uMsg == WM_CREATE)
		throw Exception(L"Failed to setup WndProxyProc");

	CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
	Window* const window = reinterpret_cast<Window * const>(createStruct->lpCreateParams);

	SetLastError(ERROR_SUCCESS);

	if (SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG>(window)) == FALSE && GetLastError() != ERROR_SUCCESS)
		throw Exception(L"Failed to set GWLP_USERDATA when handling WndStartupProc");

	if (SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG>(Window::WndProxyProc)) == FALSE && GetLastError() != ERROR_SUCCESS)
		throw Exception(L"Failed to set GWLP_WNDPROC when handling WndStartupProc with WndProxyProc");

	return window->WndProc(hWnd, uMsg, wParam, lParam);
}

LRESULT WINAPI Window::WndProxyProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Window* const wnd = reinterpret_cast<Window* const>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	return wnd->WndProc(hWnd, uMsg, wParam, lParam);
}

LRESULT Window::HandlerOnSizing(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, Window* self)
{
	RECT* windowRect = reinterpret_cast<RECT*>(lParam);

	self->windowWidth = windowRect->right - windowRect->left;
	self->windowHeight = windowRect->bottom - windowRect->top;

	RECT clientRect;
	if (GetClientRect(self->hWnd, &clientRect) == FALSE)
		throw Exception(L"Failed to recalculate client window rect when sizing");

	self->clientWidth = clientRect.right - clientRect.left;
	self->clientHeight = clientRect.bottom - clientRect.top;

	if (self->REDRAW_WINDOW_CONTENT_WHEN_SIZING)
		InvalidateRect(self->hWnd, NULL, FALSE);

	return 0;
}

void Window::CalculateWindowProperties()
{
	RECT clientRect;
	if (GetClientRect(this->hWnd, &clientRect) == FALSE)
		throw Exception(L"Failed to get client rect when calculating window properties");
	
	RECT windowRect;
	if (GetWindowRect(this->hWnd, &windowRect) == FALSE)
		throw Exception(L"Failed to get window rect when calculating window properties");

	this->clientWidth = clientRect.right - clientRect.left;
	this->clientHeight = clientRect.bottom - clientRect.top;

	this->windowWidth = windowRect.right - windowRect.left;
	this->windowHeight = windowRect.bottom - windowRect.top;
}

LRESULT Window::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto itrHandler = this->handlers.find(uMsg);

	if (itrHandler == this->handlers.end())
		return DefWindowProc(hWnd, uMsg, wParam, lParam);

	return itrHandler->second->handler(hWnd, uMsg, wParam, lParam, itrHandler->second->objectPointer);
}

Window::Window(std::wstring windowName)
{
	this->windowName = windowName;
	this->hInst = GetModuleHandle(NULL);
	this->hWnd = NULL;
	this->clientWidth = 0;
	this->clientHeight = 0;
	this->windowWidth = 0;
	this->windowHeight = 0;

	this->SetActionHandler(WM_SIZING, reinterpret_cast<Window::ActionHandler>(Window::HandlerOnSizing), reinterpret_cast<void*>(this));
}

void Window::Create()
{
	if (this->hWnd != NULL)
		throw Exception(L"Failed to Create window: window has been already created");

	this->hWnd = CreateWindow(Window::WND_CLASS_NAME, windowName.c_str(), WS_VISIBLE | WS_OVERLAPPEDWINDOW, 0, 0,
		Window::DEFAULT_WINDOW_WIDTH, Window::DEFAULT_WINDOW_HEIGHT, NULL, NULL,
		this->hInst, reinterpret_cast<LPVOID>(this));

	if (this->hWnd == INVALID_HANDLE_VALUE)
		throw Exception(L"Failed to create main window");

	this->CalculateWindowProperties();
}

void Window::SetWindowName(std::wstring windowName)
{
	this->windowName = windowName;
	if (SetWindowText(this->hWnd, windowName.c_str()) == FALSE)
		throw Exception(L"Failed to setup new window name");
}

std::wstring Window::GetWindowName()
{
	return this->windowName;
}

void Window::SetActionHandler(UINT uMsg, ActionHandler handler, void* object)
{
	ActionHandlerAndUserObject* entry = new ActionHandlerAndUserObject();

	entry->handler = handler;
	entry->objectPointer = object;

	auto itrHandler = this->handlers.find(uMsg);

	if (itrHandler != this->handlers.end())
		delete itrHandler->second;

	this->handlers.insert_or_assign(uMsg, entry);
}

int32_t Window::GetClientWidth()
{
	return this->clientWidth;
}

int32_t Window::GetClientHeight()
{
	return this->clientHeight;
}
