#pragma once
#include "Engine/Base/WindowAppli.h"
#include "Engine/Base/GraphicsCore.h"
#include "Engine/Base/DescriptorHeap.h"
#include "Engine/Externals/imgui/imgui.h"
#include <memory>

class ImGuiManager {
public:
	static ImGuiManager* GetInstance();

	void Initialize();

	void Begin();

	void End();

	void Draw();

	void ShutDown();

private:
	ImGuiManager() = default;
	~ImGuiManager() = default;
	ImGuiManager(const ImGuiManager&) = delete;
	ImGuiManager& operator=(const ImGuiManager&) = delete;

private:
	WindowAppli* application_ = nullptr;

	GraphicsCore* graphicsCore_ = nullptr;

	std::unique_ptr<DescriptorHeap> srvDescriptorHeap_ = nullptr;
};

