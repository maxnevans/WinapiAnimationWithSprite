#include "Animation.h"
#include "Exception.h"

Animation::Animation(uint32_t countSteps)
{
	this->countSteps = countSteps;
	this->isStarted = false;
	this->currentStep = 0;
	this->prevValue = 0;
}

void Animation::Start()
{
	this->currentStep = 0;
	this->isStarted = true;
	this->prevValue = 0;
}

bool Animation::isAnimating()
{
	return this->isStarted;
}

float Animation::GetValue()
{
	if (!this->isStarted)
		throw Exception(L"you have not started the animation yet");

	float x = this->currentStep / static_cast<float>(this->countSteps);

	return this->timeFunction(x);
}

float Animation::GetDelta()
{
	if (!this->isStarted)
		throw Exception(L"you have not started the animation yet");

	float x = this->currentStep / static_cast<float>(this->countSteps);
	float currentValue = this->timeFunction(x);
	float deltaValue = currentValue - this->prevValue;

	this->prevValue = currentValue;

	return deltaValue;
}

void Animation::Step()
{
	if (this->currentStep == this->countSteps)
		this->isStarted = false;

	this->currentStep++;
}

float Animation::timeFunction(float x)
{
	return x * x;
}
