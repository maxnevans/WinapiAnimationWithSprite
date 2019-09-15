#pragma once
#include <stdint.h>
#include <string>

class Sprite
{
public:
	Sprite(int32_t x, int32_t y, int32_t width, int32_t height);
	int32_t GetX();
	int32_t GetY();
	int32_t GetRightBorder();
	int32_t GetBottomBorder();
	int32_t GetWidth();
	int32_t GetHeight();
	void* GetImageHandle();
	void SetImageHandle(void* imageHandle);
	bool HasImage();

	void MoveByCountX(int32_t countX);
	void MoveByCountY(int32_t countY);
	void MoveToX(int32_t x);
	void MoveToY(int32_t y);

private:
	int32_t x, y, width, height;
	void* imageHandle;

};

