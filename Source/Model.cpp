#include "Model.h"
#include <windowsx.h>
#include <cmath>

Model::Model(Window* const window)
	:
	window(window)
{
	this->spriteImage = Gdiplus::Image::FromFile(L"sprite.png");
	if (Gdiplus::ImageType::ImageTypeUnknown == this->spriteImage->GetType())
	{
		delete this->spriteImage;
		this->spriteImage = nullptr;
		
		this->sprite = new Sprite(DEFAULT_SPRITE_X_COORD, DEFAULT_SPRITE_Y_COORD,
			static_cast<int32_t>(this->DEFAULT_SPRITE_WIDTH),
			static_cast<int32_t>(this->DEFAULT_SPRITE_WIDTH));
	}
	else
	{
		this->sprite = new Sprite(DEFAULT_SPRITE_X_COORD, DEFAULT_SPRITE_Y_COORD,
			static_cast<int32_t>(this->spriteImage->GetWidth()), 
			static_cast<int32_t>(this->spriteImage->GetHeight()));
		this->sprite->SetImageHandle(reinterpret_cast<void*>(this->spriteImage));
	}
	this->bumpAnimation = new Animation(static_cast<uint32_t>(BUMP_ANIMATION_DURATION / BUMP_ANIMATION_TIMER_DELTA));
	this->SetHandlers(window);
	this->window->Create();

	this->bump = Direction::NONE;

	this->bouncingMode = false;
	this->horizontalVelocity = 0;
	this->horizontalVelocity = 0;
	this->countBounces = 0;
	this->bounce = Direction::NONE;
}

Model::~Model()
{
	delete this->bumpAnimation;
	delete this->sprite;
}

void Model::RedrawSprite(Gdiplus::Graphics& graphics, Sprite& sprite)
{
	using namespace Gdiplus;

	SolidBrush sb(Color::Color(DEFAULT_SPRITE_COLOR));

	if (sprite.HasImage())
	{
		graphics.DrawImage(reinterpret_cast<Image*>(sprite.GetImageHandle()), 
			static_cast<INT>(sprite.GetX()), static_cast<INT>(sprite.GetY()));
	}
	else
	{
		graphics.FillRectangle(&sb, static_cast<INT>(sprite.GetX()),
			static_cast<INT>(sprite.GetY()), static_cast<INT>(sprite.GetWidth()),
			static_cast<INT>(sprite.GetHeight()));
	}
}

void Model::OnMouseWheel(HWND hWnd, Animation& animation, Sprite& sprite, short amountScroll, bool isHorizontal)
{
	if (animation.isAnimating())
		return;

	if (this->bouncingMode)
		this->StopBouncingAnimation(hWnd);

	if (this->CheckForMouseDelta(amountScroll))
	{
		amountScroll /= WHEEL_DELTA;
		amountScroll = static_cast<short>(amountScroll * this->MOUSE_SENSITIVITY_FACTOR);
		this->INVERSE_MOUSE_WHEEL ? amountScroll = -amountScroll : NULL;
	}
		

	if (isHorizontal)
		sprite.MoveByCountX(amountScroll);
	else
		sprite.MoveByCountY(amountScroll);

	if (this->RecalculateDirection(this->bump, sprite))
	{
		this->StartBumpAnimation(hWnd, animation);
		this->ChangeWindowNameOnBump();
	}
}

bool Model::RecalculateDirection(Direction& direction, Sprite& sprite)
{
	if (sprite.GetX() < 0)
	{
		sprite.MoveToX(0);
		direction = Direction::LEFT;
	}
	else if (sprite.GetRightBorder() > window->GetClientWidth())
	{
		sprite.MoveToX(window->GetClientWidth() - sprite.GetWidth());
		direction = Direction::RIGHT;
	}
	else if (sprite.GetY() < 0)
	{
		sprite.MoveToY(0);
		direction = Direction::TOP;
	}
	else if (sprite.GetBottomBorder() > window->GetClientHeight())
	{
		sprite.MoveToY(window->GetClientHeight() - sprite.GetHeight());
		direction = Direction::BOTTOM;
	}
	else
	{
		direction = Direction::NONE;
		return false;
	}

	return true;
}

void Model::StartBumpAnimation(HWND hWnd, Animation& animation)
{
	if (SetTimer(hWnd, BUMP_ANIMATION_TIMER_ID, BUMP_ANIMATION_TIMER_DELTA, NULL) == FALSE)
		throw new Exception(L"Failed to start animation timer");

	this->SaveWindowName();
	animation.Start();
}

void Model::StopBumpAnimation(HWND hWnd, Animation& animation)
{
	if (KillTimer(hWnd, BUMP_ANIMATION_TIMER_ID) == FALSE)
		throw new Exception(L"Failed to stop animation timer");

	this->RestoreWindowName();
}

void Model::AnimateBump(HWND hWnd, Animation& animation, Sprite& sprite)
{
	float delta = animation.GetDelta();
	float curVal = animation.GetValue();
	animation.Step();
	int32_t moveBy = static_cast<int32_t>(SPRITE_BUMP_ANIMATION_MOVE_DELTA * delta);

	switch (this->bump)
	{
		case Direction::TOP:
			sprite.MoveByCountY(moveBy);
			break;
		case Direction::BOTTOM:
			sprite.MoveByCountY(-moveBy);
			break;
		case Direction::LEFT:
			sprite.MoveByCountX(moveBy);
			break;
		case Direction::RIGHT:
			sprite.MoveByCountX(-moveBy);
			break;
	}

	if (!animation.isAnimating())
	{
		this->bump = Direction::NONE;
		this->StopBumpAnimation(hWnd, animation);
		this->ChangeWindowNameOnBump();
	}
}

bool Model::CheckForMouseDelta(short amountScroll)
{
	return amountScroll % WHEEL_DELTA == 0;
}

void Model::ChangeWindowNameOnBump()
{
	switch (bump)
	{
		case Direction::TOP:
			savedWindowName = this->window->GetWindowName();
			window->SetWindowName(this->WINDOW_TITLE_BUMP_TOP);
			break;
		case Direction::BOTTOM:
			savedWindowName = this->window->GetWindowName();
			window->SetWindowName(this->WINDOW_TITLE_BUMP_BOTTOM);
			break;
		case Direction::LEFT:
			savedWindowName = this->window->GetWindowName();
			window->SetWindowName(this->WINDOW_TITLE_BUMP_LEFT);
			break;
		case Direction::RIGHT:
			savedWindowName = this->window->GetWindowName();
			window->SetWindowName(this->WINDOW_TITLE_BUMP_RIGHT);
			break;
	}
}

void Model::StartBouncingAnimation(HWND hWnd)
{
	this->bouncingMode = true;
	this->SaveWindowName();
	this->CalculateBouncingVelocities();
	this->countBounces = 0;

	if (SetTimer(hWnd, this->BOUNCE_ANIMATION_TIMER_ID, this->BOUNCE_ANIMATION_TIMER_DELTA, NULL) == FALSE)
		throw Exception(L"Failed to setup bouncing animation timer");
}

void Model::StopBouncingAnimation(HWND hWnd)
{
	if (KillTimer(hWnd, this->BOUNCE_ANIMATION_TIMER_ID) == FALSE)
		throw Exception(L"Failed to kill bounce animation timer");

	this->bouncingMode = false;
	this->RestoreWindowName();
}

void Model::AnimateBounce(HWND hWnd, Sprite& sprite)
{
	if (!this->IsSpriteInsideWindow(*this->window, sprite))
	{
		this->countBounces++;

		if (!this->RecalculateDirection(this->bounce, sprite))
			throw Exception(L"Bounce has been detected but direction has not been calculated");

		this->ChangeVelocityBasedOnDirection(this->bounce);
		this->ChangeWindowNameOnBounce();
	}

	sprite.MoveByCountX(this->horizontalVelocity);
	sprite.MoveByCountY(this->verticalVelocity);
}

void Model::CalculateBouncingVelocities()
{
	int direction = std::rand();

	short horizontalGravity = (direction >> sizeof(short));
	horizontalGravity = horizontalGravity / std::abs(horizontalGravity);

	short verticalGravity = direction & 0xFFFF;
	verticalGravity = verticalGravity / std::abs(verticalGravity);

	this->verticalVelocity = verticalGravity * this->BOUNCE_ANIMATION_SPRITE_VELOCITY;
	this->horizontalVelocity = horizontalGravity * this->BOUNCE_ANIMATION_SPRITE_VELOCITY;
}

bool Model::IsSpriteInsideWindow(Window& window, Sprite& sprite)
{
	return !(sprite.GetX() < 0 || sprite.GetRightBorder() > window.GetClientWidth()
		|| sprite.GetY() < 0 || sprite.GetBottomBorder() > window.GetClientHeight());
}

void Model::ChangeWindowNameOnBounce()
{
	this->window->SetWindowName(std::wstring(this->WINDOW_TITLE_BOUNCE_OFF_COUNT) 
		+ std::to_wstring(this->countBounces) + L".");
}

void Model::ChangeVelocityBasedOnDirection(Direction& direction)
{
	switch (direction)
	{
		case Direction::TOP:
		case Direction::BOTTOM:
			this->verticalVelocity *= -1;
			break;
		case Direction::LEFT:
		case Direction::RIGHT:
			this->horizontalVelocity *= -1;
			break;
		default:
			throw Exception(L"Bounce place is not set, first calculate bounce place");
	}
}

void Model::TrasportSpriteToCoords(int x, int y)
{
	this->sprite->MoveToX(static_cast<int32_t>(x));
	this->sprite->MoveToY(static_cast<int32_t>(y));
}

void Model::SaveWindowName()
{
	this->savedWindowName = this->window->GetWindowName();
}

void Model::RestoreWindowName()
{
	this->window->SetWindowName(this->savedWindowName);
}

void Model::SetHandlers(Window* window)
{
	this->SetHandlerInWindow(WM_CREATE, Model::HandlerOnCreate);
	this->SetHandlerInWindow(WM_PAINT, Model::HandlerOnPaint);
	this->SetHandlerInWindow(WM_MOUSEWHEEL, Model::HandlerOnMouseWheel);
	this->SetHandlerInWindow(WM_LBUTTONDOWN, Model::HandlerOnLMouseButton);
	this->SetHandlerInWindow(WM_TIMER, Model::HandlerOnTimer);
	this->SetHandlerInWindow(WM_CLOSE, Model::HandlerOnClose);
	this->SetHandlerInWindow(WM_DESTROY, Model::HandlerOnDestroy);
}

void Model::SetHandlerInWindow(UINT uMsg, ActionHandler handler)
{
	window->SetActionHandler(uMsg, reinterpret_cast<Window::ActionHandler>(handler), reinterpret_cast<void*>(this));
}

LRESULT Model::HandlerOnTimer(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, Model* self)
{
	if (wParam == self->BUMP_ANIMATION_TIMER_ID)
	{
		self->AnimateBump(hWnd, *self->bumpAnimation, *self->sprite);
		InvalidateRect(hWnd, NULL, FALSE);
	}

	if (wParam == self->BOUNCE_ANIMATION_TIMER_ID)
	{
		self->AnimateBounce(hWnd, *self->sprite);
		InvalidateRect(hWnd, NULL, FALSE);
	}

	return 0;
}

LRESULT Model::HandlerOnCreate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, Model* self)
{
	InvalidateRect(hWnd, NULL, FALSE);

	return 0;
}

LRESULT Model::HandlerOnPaint(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, Model* self)
{
	using namespace Gdiplus;

	HDC hdc;
	PAINTSTRUCT ps;

	hdc = BeginPaint(hWnd, &ps);
	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hBmp = CreateCompatibleBitmap(hdc, self->window->GetClientWidth(), self->window->GetClientHeight());
	SelectObject(hdcMem, hBmp);
	Graphics graphics(hdcMem);

	SolidBrush sb(Color::Color(self->DEFAULT_BG_COLOR));
	graphics.FillRectangle(&sb, 0, 0, self->window->GetClientWidth(), self->window->GetClientHeight());

	self->RedrawSprite(graphics, *self->sprite);

	BitBlt(hdc, 0, 0, self->window->GetClientWidth(), self->window->GetClientHeight(), hdcMem, 0, 0, SRCCOPY);
	DeleteDC(hdcMem);
	DeleteObject(hBmp);
	EndPaint(hWnd, &ps);

	return 0;
}

LRESULT Model::HandlerOnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, Model* self)
{
	self->OnMouseWheel(hWnd, *self->bumpAnimation, *self->sprite, GET_WHEEL_DELTA_WPARAM(wParam),
		GET_KEYSTATE_WPARAM(wParam) & MK_SHIFT);
	InvalidateRect(hWnd, NULL, FALSE);

	return 0;
}

LRESULT Model::HandlerOnClose(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, Model* self)
{
	DestroyWindow(hWnd);

	return 0;
}

LRESULT Model::HandlerOnDestroy(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, Model* self)
{
	PostQuitMessage(0);

	return 0;
}

LRESULT Model::HandlerOnLMouseButton(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, Model* self)
{
	self->TrasportSpriteToCoords(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

	if (!self->bouncingMode)
		self->StartBouncingAnimation(hWnd);

	return 0;
}
