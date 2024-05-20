#pragma once
#include "Base/WindowAppli.h"
#include "Base/GraphicsCore.h"
#include "Base/DescriptorHeap.h"
#include "Externals/imgui/imgui.h"
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

