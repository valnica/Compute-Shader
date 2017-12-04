#pragma once

#include <Windows.h>

class FPSTimer
{
private:
	//周波数
	LARGE_INTEGER freq_;
	LARGE_INTEGER start_;
	LARGE_INTEGER end_;

	//最大FPSの設定
	int maxFPS_;

	//前フレームとの時間差
	float deltaTime_;
	
	//何回fpsをカウントしたか
	int fpsCounter_;

	//1秒のカウント
	float time_;
	
	//平均FPS
	int averageFPS_;



public:
	FPSTimer(int maxFPS);
	~FPSTimer();

	void Wait();

	float TotalTime()
	{
		return deltaTime_;
	}

	int FPS()
	{
		return averageFPS_;
	}
};

