#pragma once

#include <Windows.h>

class FPSTimer
{
private:
	//���g��
	LARGE_INTEGER freq_;
	LARGE_INTEGER start_;
	LARGE_INTEGER end_;

	//�ő�FPS�̐ݒ�
	int maxFPS_;

	//�O�t���[���Ƃ̎��ԍ�
	float deltaTime_;
	
	//����fps���J�E���g������
	int fpsCounter_;

	//1�b�̃J�E���g
	float time_;
	
	//����FPS
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

