#pragma once
#include <chrono>
#include <thread>

class FramePerSsecondGenerate
{
public:
	void Initialize();

	void Update();

private:
	std::chrono::steady_clock::time_point reference_{};
};

