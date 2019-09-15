#pragma once
#include <stdint.h>

class Animation
{
public:
	Animation(uint32_t countSteps);
	void Start();
	bool isAnimating();
	float GetValue();
	float GetDelta();
	void Step();

private:
	float timeFunction(float x);

private:
	bool isStarted;
	uint32_t currentStep;
	uint32_t countSteps;
	float prevValue;
};

