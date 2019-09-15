#include "Sprite.h"
#include "Exception.h"

Sprite::Sprite(int32_t x, int32_t y, int32_t width, int32_t height)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->imageHandle = nullptr;
}

int32_t Sprite::GetX()
{
	return this->x;
}

int32_t Sprite::GetY()
{
	return this->y;
}

int32_t Sprite::GetRightBorder()
{
	return this->x + this->width;
}

int32_t Sprite::GetBottomBorder()
{
	return this->y + this->height;
}

int32_t Sprite::GetWidth()
{
	return this->width;
}

int32_t Sprite::GetHeight()
{
	return this->height;
}

void* Sprite::GetImageHandle()
{
	return this->imageHandle;
}

void Sprite::SetImageHandle(void* imageHandle)
{
	this->imageHandle = imageHandle;
}

bool Sprite::HasImage()
{
	return this->imageHandle != nullptr;
}

void Sprite::MoveByCountX(int32_t countX)
{
	this->x += countX;
}

void Sprite::MoveByCountY(int32_t countY)
{
	this->y += countY;
}

void Sprite::MoveToX(int32_t x)
{
	this->x = x;
}

void Sprite::MoveToY(int32_t y)
{
	this->y = y;
}
