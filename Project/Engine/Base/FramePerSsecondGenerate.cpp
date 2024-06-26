#include "FramePerSsecondGenerate.h"

void FramePerSsecondGenerate::Initialize()
{
	//現在時間を記録する
	reference_ = std::chrono::steady_clock::now();
}

void FramePerSsecondGenerate::Update()
{
	//1/60秒より少し短い
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));
	//1/60秒
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));

	//現在時間を取得する
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	//1/60経っていない場合
	if (elapsed < kMinCheckTime) {
		//1/60秒経過するまでスリープを繰り返し
		while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
			//1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	//現在の時間を記録する
	reference_ = std::chrono::steady_clock::now();
}