#pragma once
#include "WindowAppli.h"
#include "Buffer/ColorBuffer.h"
#include "Buffer/DepthBuffer.h"
#include <array>
#include <memory>
#include <dxgi1_6.h>

class Display
{
public:
	void Initialize(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* commandQueue);

	void Present();

	ColorBuffer& GetCurrentBuffer();

private:
	void CreateSwapChain(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* commandQueue);

	void CreateResources();

private:
	WindowAppli* ｗindowAppli_ = nullptr;

	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;

	std::array<std::unique_ptr<ColorBuffer>, 2> swapChainResources_;
};

