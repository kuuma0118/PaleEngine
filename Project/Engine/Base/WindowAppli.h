#pragma once
#include <Windows.h>
#include <cstdint>

class WindowAppli
{
public:
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

	static WindowAppli* GetInstance();

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	void CreateGameWindow(const wchar_t* title, int32_t clientWidth, int32_t clientHeight);

	void CloseGameWindow();

	bool ProcessMessage();

	HWND GetHwnd() const { return hwnd_; };

	HINSTANCE GetHInstance() { return wc_.hInstance; };

private:
	WindowAppli() = default;
	~WindowAppli() = default;
	WindowAppli(const WindowAppli&) = delete;
	const WindowAppli& operator=(const WindowAppli&) = delete;

private:
	WNDCLASS wc_{};

	RECT wrc_{};

	HWND hwnd_{};
};

