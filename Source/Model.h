#pragma once
#include "Sprite.h"
#include "Animation.h"
#include "Exception.h"
#include "Window.h"
#include <windows.h>
#include <gdiplus.h>

class Model
{
private:
	enum Direction
	{
		NONE, LEFT, RIGHT, TOP, BOTTOM
	};

	typedef LRESULT(*ActionHandler)(HWND, UINT, WPARAM, LPARAM, Model*);

public:
	Model(Window* const window);
	~Model();
	Model(const Model&) = delete;

private:
	static LRESULT HandlerOnCreate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, Model* self);
	static LRESULT HandlerOnTimer(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, Model* self);
	static LRESULT HandlerOnPaint(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, Model* self);
	static LRESULT HandlerOnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, Model* self);
	static LRESULT HandlerOnClose(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, Model* self);
	static LRESULT HandlerOnDestroy(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, Model* self);
	static LRESULT HandlerOnLMouseButton(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, Model* self);

	void SetHandlers(Window* window);
	void SetHandlerInWindow(UINT uMsg, ActionHandler handler);

	void RedrawSprite(Gdiplus::Graphics& graphics, Sprite& sprite);
	void OnMouseWheel(HWND hWnd, Animation& animation, Sprite& sprite, short amountScroll, bool isHorizontal);
	bool RecalculateDirection(Direction& direction, Sprite& sprite);
	void StartBumpAnimation(HWND hWnd, Animation& animation);
	void StopBumpAnimation(HWND hWnd, Animation& animation);
	void AnimateBump(HWND hWnd, Animation& animation, Sprite& sprite);
	bool CheckForMouseDelta(short amountScroll);
	void ChangeWindowNameOnBump();
	void StartBouncingAnimation(HWND hWnd);
	void StopBouncingAnimation(HWND hWnd);
	void AnimateBounce(HWND hWnd, Sprite& sprite);
	void CalculateBouncingVelocities();
	bool IsSpriteInsideWindow(Window& window, Sprite& sprite);
	void ChangeWindowNameOnBounce();
	void ChangeVelocityBasedOnDirection(Direction& direction);
	void TrasportSpriteToCoords(int x, int y);
	void SaveWindowName();
	void RestoreWindowName();

private:

	static constexpr const int BUMP_ANIMATION_TIMER_ID = 0xF1;
	static constexpr const int BUMP_ANIMATION_TIMER_DELTA = 8;
	static constexpr const int BOUNCE_ANIMATION_TIMER_ID = 0xF2;
	static constexpr const int BOUNCE_ANIMATION_TIMER_DELTA = 8;
	static constexpr const Gdiplus::ARGB DEFAULT_BG_COLOR = 0xFF62A594;
	static constexpr const int DEFAULT_SPRITE_X_COORD = 0;
	static constexpr const int DEFAULT_SPRITE_Y_COORD = 0;
	static constexpr const int DEFAULT_SPRITE_WIDTH = 100;
	static constexpr const int DEFAULT_SPRITE_HEIGHT = 100;
	static constexpr const Gdiplus::ARGB DEFAULT_SPRITE_COLOR = 0xFFDBDBDB;
	static constexpr const int BUMP_ANIMATION_DURATION = 65;
	static constexpr const int SPRITE_BUMP_ANIMATION_MOVE_DELTA = 100;
	static constexpr const wchar_t* SPRITE_IMAGE_FILENAME = L"sprite.bmp";
	static constexpr const int BOUNCE_ANIMATION_SPRITE_VELOCITY = 10;

	static constexpr const float MOUSE_SENSITIVITY_FACTOR = 10.5;
	static constexpr const bool INVERSE_MOUSE_WHEEL = true;

	static constexpr const wchar_t* WINDOW_TITLE_BUMP_TOP = L"Sprite bumped on top";
	static constexpr const wchar_t* WINDOW_TITLE_BUMP_BOTTOM = L"Sprite bumped on bottom";
	static constexpr const wchar_t* WINDOW_TITLE_BUMP_LEFT = L"Sprite bumped on left";
	static constexpr const wchar_t* WINDOW_TITLE_BUMP_RIGHT = L"Sprite bumped on right";
	static constexpr const wchar_t* WINDOW_TITLE_BOUNCE_OFF_COUNT = L"Bouncing off mode. Count bounced time: ";

	
	int horizontalVelocity;
	int verticalVelocity;
	bool bouncingMode;
	int countBounces;
	Direction bounce;

	Direction bump;
	Animation* bumpAnimation;

	Sprite* sprite;
	Gdiplus::Image* spriteImage;

	Window* const window;
	std::wstring savedWindowName;
};

