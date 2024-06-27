#pragma once
#include <Windows.h>
#include <string>
#include <format>

namespace Logs 
{
	void Log(const std::string& message);

	std::wstring ConvertString(const std::string& str);

	std::string ConvertString(const std::wstring& str);
}

