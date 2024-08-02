#include "Renderer.h"
#include "GraphicsDirectionCenter.h"
#include "Engine/Utility/ShaderCompiler.h"
#include <algorithm>
#include <cassert>

//実体定義
Renderer* Renderer::instance_ = nullptr;

Renderer* Renderer::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new Renderer();
	}
	return instance_;
}

void Renderer::Destroy()
{
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

void Renderer::Initialize()
{
	//シーンを描画するリソースの作成
	sceneColorBuffer_ = std::make_unique<ColorBuffer>();
	sceneColorBuffer_->Create(WindowAppli::kClientWidth, WindowAppli::kClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

	//線形深度を描画するリソースの作成
	float depthClearColor[] = { 1.0f,1.0f,1.0f,1.0f };
	linearDepthColorBuffer_ = std::make_unique<ColorBuffer>();
	linearDepthColorBuffer_->Create(WindowAppli::kClientWidth, WindowAppli::kClientHeight, DXGI_FORMAT_R32_FLOAT, depthClearColor);

	//深度バッファの作成
	sceneDepthBuffer_ = std::make_unique<DepthBuffer>();
	sceneDepthBuffer_->Create(WindowAppli::kClientWidth, WindowAppli::kClientHeight, DXGI_FORMAT_D24_UNORM_S8_UINT);

	//LightManagerを作成
	lightManager_ = LightManager::GetInstance();

	//スキニングモデル用のPSOの作成	
	CreateSkinningModelPipelineState();

	//モデル用のPSOの作成
	CreateModelPipelineState();

	//スプライト用のPSOの作成
	CreateSpritePipelineState();

	//パーティクル用のPSOの作成
	CreateParticlePipelineState();

	//DebugObject用のPSOの作成	
	CreateDebugPipelineState();

	//Skybox用のPSOの作成
	CreateSkyboxPipelineState();
}

void Renderer::AddObject(D3D12_VERTEX_BUFFER_VIEW vertexBufferView, D3D12_VERTEX_BUFFER_VIEW influenceBufferView,
	D3D12_INDEX_BUFFER_VIEW indexBufferView, D3D12_GPU_VIRTUAL_ADDRESS materialCBV, D3D12_GPU_VIRTUAL_ADDRESS worldTransformCBV,
	D3D12_GPU_VIRTUAL_ADDRESS cameraCBV, D3D12_GPU_DESCRIPTOR_HANDLE textureSRV, D3D12_GPU_DESCRIPTOR_HANDLE matrixPaletteSRV, UINT indexCount, DrawPass drawPass)
{
	SortObject sortObject{};
	sortObject.vertexBufferView = vertexBufferView;
	sortObject.influenceBufferView = influenceBufferView;
	sortObject.indexBufferView = indexBufferView;
	sortObject.materialCBV = materialCBV;
	sortObject.worldTransformCBV = worldTransformCBV;
	sortObject.cameraCBV = cameraCBV;
	sortObject.textureSRV = textureSRV;
	sortObject.matrixPaletteSRV = matrixPaletteSRV;
	sortObject.indexCount = indexCount;
	sortObject.type = drawPass;
	sortObjects_.push_back(sortObject);
}

void Renderer::AddDebugObject(D3D12_VERTEX_BUFFER_VIEW vertexBufferView, D3D12_GPU_VIRTUAL_ADDRESS worldTransformCBV, D3D12_GPU_VIRTUAL_ADDRESS cameraCBV, UINT vertexCount)
{
	DebugObject debugObject{};
	debugObject.vertexBufferView = vertexBufferView;
	debugObject.worldTransformCBV = worldTransformCBV;
	debugObject.cameraCBV = cameraCBV;
	debugObject.vertexCount = vertexCount;
	debugObjects_.push_back(debugObject);
}

void Renderer::Render()
{
	//並び替える
	Sort();

	//描画パスを設定
	DrawPass currentRenderingType = Opaque;

	//コマンドリストを取得
	CommandContext* commandContext = GraphicsDirectionCenter::GetInstance()->GetCommandContext();

	//RootSignatureを設定
	commandContext->SetRootSignature(skinningModelRootSignature_);

	//PipelineStateを設定
	commandContext->SetPipelineState(skinningModelPipelineStates_[currentRenderingType]);

	//DirectionalLightを設定
	commandContext->SetConstantBuffer(kDirectionalLight, lightManager_->GetConstantBuffer()->GetGpuVirtualAddress());

	//EnvironmentTextureを設定	
	commandContext->SetDescriptorTable(kEnvironmentTexture, lightManager_->GetEnvironmentTexture()->GetSRVHandle());

	for (const SortObject& sortObject : sortObjects_) {
		//不透明オブジェクトに切り替わったらPSOも変える
		if (currentRenderingType != sortObject.type) {
			currentRenderingType = sortObject.type;
			commandContext->SetPipelineState(skinningModelPipelineStates_[currentRenderingType]);
		}
			//VertexBufferViewを設定
			D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
				sortObject.vertexBufferView,//VertexDataのVBV
				sortObject.influenceBufferView//InfluenceのVBV
			};
		commandContext->SetVertexBuffers(0, 2, vbvs);
		//形状を設定。PSOに設定しているものとは別。同じものを設定すると考えておけば良い
		commandContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//IndexBufferViewを設定	//マテリアルを設定
		commandContext->SetIndexBuffer(sortObject.indexBufferView);
		//マテリアルを設定
		commandContext->SetConstantBuffer(kMaterial, sortObject.materialCBV);
		//WorldTransformを設定
		commandContext->SetConstantBuffer(kWorldTransform, sortObject.worldTransformCBV);
		//Cameraを設定
		commandContext->SetConstantBuffer(kCamera, sortObject.cameraCBV);
		//Textureを設定
		commandContext->SetDescriptorTable(kTexture, sortObject.textureSRV);
		//MatrixPaletteを設定	//描画!(DrawCall/ドローコール)。
		commandContext->SetDescriptorTable(kMatrixPalette, sortObject.matrixPaletteSRV);
		//描画!(DrawCall/ドローコール)。
		commandContext->DrawIndexedInstanced(sortObject.indexCount, 1);
	}

	//オブジェクトをリセット
	sortObjects_.clear();
	//RootSignatureを設定	
	commandContext->SetRootSignature(debugRootSignature_);
	//PipelineStateを設定	
	commandContext->SetPipelineState(debugPipelineStates_[0]);
	//DebugObjectの描画	
	for (const DebugObject& debugObject : debugObjects_)
	{
		//VertexBufferViewを設定	
		commandContext->SetVertexBuffer(debugObject.vertexBufferView);
		//形状を設定。PSOに設定しているものとは別。同じものを設定すると考えておけば良い	
		commandContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		//WorldTransformを設定	
		commandContext->SetConstantBuffer(0, debugObject.worldTransformCBV);
		//Cameraを設定	
		commandContext->SetConstantBuffer(1, debugObject.cameraCBV);
		//描画!(DrawCall/ドローコール)。3頂点で1つのインスタンス。
		commandContext->DrawInstanced(debugObject.vertexCount, 1);
	}
	debugObjects_.clear();
}

void Renderer::PreDraw()
{
	//コマンドリストを取得
	CommandContext* commandContext = GraphicsDirectionCenter::GetInstance()->GetCommandContext();

	//リソースの状態遷移
	commandContext->TransitionResource(*sceneColorBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandContext->TransitionResource(*linearDepthColorBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);

	//レンダーターゲットとデプスバッファを設定
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	rtvHandles[0] = sceneColorBuffer_->GetRTVHandle();
	rtvHandles[1] = linearDepthColorBuffer_->GetRTVHandle();
	commandContext->SetRenderTargets(2, rtvHandles, sceneDepthBuffer_->GetDSVHandle());

	//レンダーターゲットをクリア
	ClearRenderTarget();

	//デプスバッファをクリア
	ClearDepthBuffer();

	//ビューポート
	D3D12_VIEWPORT viewport{};
	viewport.Width = WindowAppli::kClientWidth;
	viewport.Height = WindowAppli::kClientHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	commandContext->SetViewport(viewport);

	//シザー矩形
	D3D12_RECT scissorRect{};
	scissorRect.left = 0;
	scissorRect.right = WindowAppli::kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = WindowAppli::kClientHeight;
	commandContext->SetScissor(scissorRect);

	//DescriptorHeapを設定
	commandContext->SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, GraphicsDirectionCenter::GetInstance()->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
}

void Renderer::PostDraw()
{
	CommandContext* commandContext = GraphicsDirectionCenter::GetInstance()->GetCommandContext();
	commandContext->TransitionResource(*sceneColorBuffer_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandContext->TransitionResource(*linearDepthColorBuffer_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void Renderer::ClearRenderTarget()
{
	CommandContext* commandContext = GraphicsDirectionCenter::GetInstance()->GetCommandContext();
	commandContext->ClearColor(*sceneColorBuffer_);
	commandContext->ClearColor(*linearDepthColorBuffer_);
}

void Renderer::ClearDepthBuffer()
{
	CommandContext* commandContext = GraphicsDirectionCenter::GetInstance()->GetCommandContext();
	commandContext->ClearDepth(*sceneDepthBuffer_);
}

void Renderer::PreDrawSprites(BlendMode blendMode) {
	//コマンドリストを取得
	CommandContext* commandContext = GraphicsDirectionCenter::GetInstance()->GetCommandContext();
	//RootSignatureを設定
	commandContext->SetRootSignature(spriteRootSignature_);
	//PipelineStateを設定
	commandContext->SetPipelineState(spritePipelineStates_[blendMode]);
}

void Renderer::PostDrawSprites() {

}

void Renderer::PreDrawParticles() {
	//コマンドリストを取得
	CommandContext* commandContext = GraphicsDirectionCenter::GetInstance()->GetCommandContext();
	//RootSignatureを設定
	commandContext->SetRootSignature(particleRootSignature_);
	//PipelineStateを設定
	commandContext->SetPipelineState(particlePipelineStates_[0]);
}

void Renderer::PostDrawParticles() {

}

void Renderer::PreDrawSkybox()	
{
	//コマンドリストを取得	
	CommandContext* commandContext = GraphicsDirectionCenter::GetInstance()->GetCommandContext();
	//RootSignatureを設定	
	commandContext->SetRootSignature(skyboxRootSignature_);
	//PipelineStateを設定	
	commandContext->SetPipelineState(skyboxPipelineStates_[0]);
}
void Renderer::PostDrawSkybox()
{
}

void Renderer::CreateModelPipelineState()
{
	//RootSignatureの作成
	modelRootSignature_.Create(6, 1);

	//RootParameterを設定
	modelRootSignature_[0].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_PIXEL);
	modelRootSignature_[1].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_VERTEX);
	modelRootSignature_[2].InitAsConstantBuffer(1, D3D12_SHADER_VISIBILITY_VERTEX);
	modelRootSignature_[3].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	modelRootSignature_[4].InitAsConstantBuffer(1, D3D12_SHADER_VISIBILITY_PIXEL);
	modelRootSignature_[5].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	//StaticSamplerを設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1]{};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;_
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	modelRootSignature_.InitStaticSampler(0, staticSamplers[0], D3D12_SHADER_VISIBILITY_PIXEL);
	modelRootSignature_.Finalize();

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3]{};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	//Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = ShaderCompiler::CompileShader(L"Object3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = ShaderCompiler::CompileShader(L"Object3d.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc[2]{};
	//すべての色要素を書き込む
	blendDesc[0].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//透明オブジェクトのBlendStateの設定
	blendDesc[1].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//共通設定
	blendDesc[1].RenderTarget[0].BlendEnable = true;//ブレンドを有効にする
	blendDesc[1].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc[1].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc[1].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	//半透明合成
	blendDesc[1].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc[1].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースのアルファ値
	blendDesc[1].RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-ソースのアルファ値

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//書き込むRTVの情報
	DXGI_FORMAT rtvFormats[2];
	rtvFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvFormats[1] = DXGI_FORMAT_R32_FLOAT;

	//PSOを作成する
	for (uint32_t i = 0; i < 2; i++) {
		PipelineState newPipelineState;
		newPipelineState.SetRootSignature(&modelRootSignature_);
		newPipelineState.SetInputLayout(3, inputElementDescs);
		newPipelineState.SetVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize());
		newPipelineState.SetPixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize());
		newPipelineState.SetBlendState(blendDesc[i]);
		newPipelineState.SetRasterizerState(rasterizerDesc);
		newPipelineState.SetRenderTargetFormats(2, rtvFormats, DXGI_FORMAT_D24_UNORM_S8_UINT);
		newPipelineState.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		newPipelineState.SetSampleMask(D3D12_DEFAULT_SAMPLE_MASK);
		newPipelineState.SetDepthStencilState(depthStencilDesc);
		newPipelineState.Finalize();
		modelPipelineStates_.push_back(newPipelineState);
	}
}

void Renderer::CreateSkinningModelPipelineState()
{
	//RootSignatureの作成
	skinningModelRootSignature_.Create(7, 1);

	//RootParameterを設定
	skinningModelRootSignature_[0].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_PIXEL);
	skinningModelRootSignature_[1].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_VERTEX);
	skinningModelRootSignature_[2].InitAsConstantBuffer(1, D3D12_SHADER_VISIBILITY_VERTEX);
	skinningModelRootSignature_[3].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	skinningModelRootSignature_[4].InitAsConstantBuffer(1, D3D12_SHADER_VISIBILITY_PIXEL);
	skinningModelRootSignature_[5].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_VERTEX);
	skinningModelRootSignature_[6].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	//StaticSamplerを設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1]{};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	skinningModelRootSignature_.InitStaticSampler(0, staticSamplers[0], D3D12_SHADER_VISIBILITY_PIXEL);
	skinningModelRootSignature_.Finalize();

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[5]{};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[3].SemanticName = "WEIGHT";
	inputElementDescs[3].SemanticIndex = 0;
	inputElementDescs[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;//float32_t4
	inputElementDescs[3].InputSlot = 1;//1番目のslotのVBVのことだと伝える
	inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[4].SemanticName = "INDEX";
	inputElementDescs[4].SemanticIndex = 0;
	inputElementDescs[4].Format = DXGI_FORMAT_R32G32B32A32_SINT;//int32_t4
	inputElementDescs[4].InputSlot = 1;
	inputElementDescs[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	//Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = ShaderCompiler::CompileShader(L"SkinningObject3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = ShaderCompiler::CompileShader(L"Object3d.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc[2]{};
	//すべての色要素を書き込む
	blendDesc[0].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//透明オブジェクトのBlendStateの設定
	blendDesc[1].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//共通設定
	blendDesc[1].RenderTarget[0].BlendEnable = true;//ブレンドを有効にする
	blendDesc[1].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc[1].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc[1].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	//半透明合成
	blendDesc[1].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc[1].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースのアルファ値
	blendDesc[1].RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-ソースのアルファ値

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//書き込むRTVの情報
	DXGI_FORMAT rtvFormats[2];
	rtvFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvFormats[1] = DXGI_FORMAT_R32_FLOAT;

	//PSOを作成する
	for (uint32_t i = 0; i < 2; i++) {
		PipelineState newPipelineState;
		newPipelineState.SetRootSignature(&skinningModelRootSignature_);
		newPipelineState.SetInputLayout(5, inputElementDescs);
		newPipelineState.SetVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize());
		newPipelineState.SetPixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize());
		newPipelineState.SetBlendState(blendDesc[i]);
		newPipelineState.SetRasterizerState(rasterizerDesc);
		newPipelineState.SetRenderTargetFormats(2, rtvFormats, DXGI_FORMAT_D24_UNORM_S8_UINT);
		newPipelineState.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		newPipelineState.SetSampleMask(D3D12_DEFAULT_SAMPLE_MASK);
		newPipelineState.SetDepthStencilState(depthStencilDesc);
		newPipelineState.Finalize();
		skinningModelPipelineStates_.push_back(newPipelineState);
	}
}


void Renderer::CreateSpritePipelineState()
{
	spriteRootSignature_.Create(3, 1);
	spriteRootSignature_[0].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_PIXEL);
	spriteRootSignature_[1].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_VERTEX);
	spriteRootSignature_[2].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_PIXEL);

	//StaticSamplerを設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1]{};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	spriteRootSignature_.InitStaticSampler(0, staticSamplers[0], D3D12_SHADER_VISIBILITY_PIXEL);
	spriteRootSignature_.Finalize();

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc[6]{};
	//ブレンドなし
	blendDesc[0].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//半透明合成
	blendDesc[1].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc[1].RenderTarget[0].BlendEnable = true;//ブレンドを有効にする
	blendDesc[1].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc[1].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc[1].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	blendDesc[1].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc[1].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースのアルファ値
	blendDesc[1].RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-ソースのアルファ値

	//加算合成
	blendDesc[2].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc[2].RenderTarget[0].BlendEnable = true;//ブレンドを有効にする
	blendDesc[2].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc[2].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc[2].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	blendDesc[2].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc[2].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースの値を100%使う
	blendDesc[2].RenderTarget[0].DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う

	//減算合成
	blendDesc[3].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc[3].RenderTarget[0].BlendEnable = true;//ブレンドを有効にする
	blendDesc[3].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc[3].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc[3].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	blendDesc[3].RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;//減算
	blendDesc[3].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースの値を100%使う
	blendDesc[3].RenderTarget[0].DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う

	//乗算合成
	blendDesc[4].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc[4].RenderTarget[0].BlendEnable = true;//ブレンドを有効にする
	blendDesc[4].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc[4].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc[4].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	blendDesc[4].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc[4].RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;//ソースの値を0%使う
	blendDesc[4].RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;//デストの色を使う

	//スクリーン合成
	blendDesc[5].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc[5].RenderTarget[0].BlendEnable = true;//ブレンドを有効にする
	blendDesc[5].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc[5].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc[5].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	blendDesc[5].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc[5].RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//1-ソースの色を使う
	blendDesc[5].RenderTarget[0].DestBlend = D3D12_BLEND_ONE;//デストの色を100%使う

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	//rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = ShaderCompiler::CompileShader(L"Sprite.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = ShaderCompiler::CompileShader(L"Sprite.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//書き込むRTVの情報
	DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//PipelineStateを作成
	for (uint32_t i = 0; i < kCountOfBlendMode; i++) {
		PipelineState newPipelineState;
		newPipelineState.SetRootSignature(&spriteRootSignature_);
		newPipelineState.SetInputLayout(2, inputElementDescs);
		newPipelineState.SetVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize());
		newPipelineState.SetPixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize());
		newPipelineState.SetBlendState(blendDesc[i]);
		newPipelineState.SetRasterizerState(rasterizerDesc);
		newPipelineState.SetRenderTargetFormats(1, &rtvFormat, DXGI_FORMAT_D24_UNORM_S8_UINT);
		newPipelineState.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		newPipelineState.SetSampleMask(D3D12_DEFAULT_SAMPLE_MASK);
		newPipelineState.SetDepthStencilState(depthStencilDesc);
		newPipelineState.Finalize();
		spritePipelineStates_.push_back(newPipelineState);
	}
}

void Renderer::CreateParticlePipelineState()
{
	particleRootSignature_.Create(4, 1);
	particleRootSignature_[0].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_PIXEL);
	particleRootSignature_[1].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_VERTEX);
	particleRootSignature_[2].InitAsConstantBuffer(1, D3D12_SHADER_VISIBILITY_VERTEX);
	particleRootSignature_[3].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_PIXEL);

	//StaticSamplerを設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1]{};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	particleRootSignature_.InitStaticSampler(0, staticSamplers[0], D3D12_SHADER_VISIBILITY_PIXEL);
	particleRootSignature_.Finalize();

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//共通設定
	blendDesc.RenderTarget[0].BlendEnable = true;//ブレンドを有効にする
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	//加算合成
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースの値を100%使う
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = ShaderCompiler::CompileShader(L"Particle.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = ShaderCompiler::CompileShader(L"Particle.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を無効化する
	depthStencilDesc.DepthEnable = false;
	//書き込みしない
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//書き込むRTVの情報
	DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	PipelineState newPipelineState;
	newPipelineState.SetRootSignature(&particleRootSignature_);
	newPipelineState.SetInputLayout(3, inputElementDescs);
	newPipelineState.SetVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize());
	newPipelineState.SetPixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize());
	newPipelineState.SetBlendState(blendDesc);
	newPipelineState.SetRasterizerState(rasterizerDesc);
	newPipelineState.SetRenderTargetFormats(1, &rtvFormat, DXGI_FORMAT_D24_UNORM_S8_UINT);
	newPipelineState.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	newPipelineState.SetSampleMask(D3D12_DEFAULT_SAMPLE_MASK);
	newPipelineState.SetDepthStencilState(depthStencilDesc);
	newPipelineState.Finalize();
	particlePipelineStates_.push_back(newPipelineState);
}

void Renderer::CreateSkyboxPipelineState()
{
	skyboxRootSignature_.Create(4, 1);
	skyboxRootSignature_[0].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_PIXEL);
	skyboxRootSignature_[1].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_VERTEX);
	skyboxRootSignature_[2].InitAsConstantBuffer(1, D3D12_SHADER_VISIBILITY_VERTEX);
	skyboxRootSignature_[3].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	//StaticSamplerを設定	
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1]{};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ	
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート	
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない	
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う	
	skyboxRootSignature_.InitStaticSampler(0, staticSamplers[0], D3D12_SHADER_VISIBILITY_PIXEL);
	skyboxRootSignature_.Finalize();
	//InputLayout	
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1]{};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//BlendStateの設定	
	D3D12_BLEND_DESC blendDesc{};
	//ブレンドなし	
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//RasterizerStateの設定	
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない	
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす	
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	//Shaderをコンパイルする	
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = ShaderCompiler::CompileShader(L"Skybox.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = ShaderCompiler::CompileShader(L"Skybox.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);
	//DepthStencilStateの設定	
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//比較はするのでDepth自体は有効	
	depthStencilDesc.DepthEnable = true;
	//全ピクセルがz = 1に出力されるので、わざわざ書き込む必要がない	
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	//今までと同様に比較	
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//書き込むRTVの情報	
	DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	PipelineState newPipelineState;
	newPipelineState.SetRootSignature(&skyboxRootSignature_);
	newPipelineState.SetInputLayout(1, inputElementDescs);
	newPipelineState.SetVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize());
	newPipelineState.SetPixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize());
	newPipelineState.SetBlendState(blendDesc);
	newPipelineState.SetRasterizerState(rasterizerDesc);
	newPipelineState.SetRenderTargetFormats(1, &rtvFormat, DXGI_FORMAT_D24_UNORM_S8_UINT);
	newPipelineState.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	newPipelineState.SetSampleMask(D3D12_DEFAULT_SAMPLE_MASK);
	newPipelineState.SetDepthStencilState(depthStencilDesc);
	newPipelineState.Finalize();
	skyboxPipelineStates_.push_back(newPipelineState);
}


void Renderer::CreateDebugPipelineState()
{
	debugRootSignature_.Create(2, 0);
	debugRootSignature_[0].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_VERTEX);
	debugRootSignature_[1].InitAsConstantBuffer(1, D3D12_SHADER_VISIBILITY_VERTEX);
	debugRootSignature_.Finalize();

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//共通設定
	blendDesc.RenderTarget[0].BlendEnable = true;//ブレンドを有効にする
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	//加算合成
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースの値を100%使う
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = ShaderCompiler::CompileShader(L"DebugObject3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = ShaderCompiler::CompileShader(L"DebugObject3d.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を無効化する
	depthStencilDesc.DepthEnable = false;
	//書き込みしない
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//書き込むRTVの情報
	DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//PSOの作成
	PipelineState newPipelineState;
	newPipelineState.SetRootSignature(&debugRootSignature_);
	newPipelineState.SetInputLayout(1, inputElementDescs);
	newPipelineState.SetVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize());
	newPipelineState.SetPixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize());
	newPipelineState.SetBlendState(blendDesc);
	newPipelineState.SetRasterizerState(rasterizerDesc);
	newPipelineState.SetRenderTargetFormats(1, &rtvFormat, DXGI_FORMAT_D24_UNORM_S8_UINT);
	newPipelineState.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
	newPipelineState.SetSampleMask(D3D12_DEFAULT_SAMPLE_MASK);
	newPipelineState.SetDepthStencilState(depthStencilDesc);
	newPipelineState.Finalize();
	debugPipelineStates_.push_back(newPipelineState);
}

void Renderer::Sort()
{
	struct { bool operator()(const SortObject& a, const SortObject& b)const { return a.type < b.type; } } Cmp;
	std::sort(sortObjects_.begin(), sortObjects_.end(), Cmp);
}