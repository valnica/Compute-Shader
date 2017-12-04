#include "FPSTimer.h"



FPSTimer::FPSTimer(int maxFPS)
	:maxFPS_(maxFPS), deltaTime_(0.0f)
	, fpsCounter_(0),time_(0.0f), averageFPS_(maxFPS_)
{
	QueryPerformanceFrequency(&freq_);
}

FPSTimer::~FPSTimer()

{
}

void FPSTimer::Wait()
{
	double microSec = 1000000 / (double)freq_.QuadPart;
	float totalTime = 0.0f;

	while (totalTime < 1000000.0f / maxFPS_)
	{
		QueryPerformanceCounter(&end_);
		totalTime = (end_.QuadPart - start_.QuadPart) * microSec;
	}

	deltaTime_ = totalTime / 1000000;

	time_ += deltaTime_;
	fpsCounter_++;

	if (time_ > 1.0f)
	{
		averageFPS_ = fpsCounter_;
		time_ = 0;
		fpsCounter_ = 0;
	}

	QueryPerformanceCounter(&start_);
}