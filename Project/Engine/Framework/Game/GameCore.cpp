#include "GameDirectionCenter.h"
#include "Engine/Utility/GlobalVariables.h"
#include "Engine/Utility/Random.h"

void GameDirectionCenter::Initialize()
{
	//Applicationの初期化
	windowAppli_ = WindowAppli::GetInstance();
	windowAppli_->CreateGameWindow(L"DirectXGame", WindowAppli::kClientWidth, WindowAppli::kClientHeight);

	//GraphicsCoreの初期化
	graphicsCore_ = GraphicsDirectionCenter::GetInstance();
	graphicsCore_->Initialize();

	//TextureManagerの初期化
	textureManager_ = TextureManager::GetInstance();
	textureManager_->Initialize();

	//ImGuiManagerの初期化
	imguiManager_ = ImGuiManager::GetInstance();
	imguiManager_->Initialize();

	//Rendererの初期化
	renderer_ = Renderer::GetInstance();
	renderer_->Initialize();

	//Inputの初期化
	input_ = Input::GetInstance();
	input_->Initialize();

	//Audioの初期化
	audio_ = Audio::GetInstance();
	audio_->Initialize();

	//PostEffectsの初期化
	postEffects_ = PostEffects::GetInstance();
	postEffects_->Initialize();

	//SceneManagerのインスタンスを取得
	sceneManager_ = SceneManager::GetInstance();

	//ModelManagerの初期化
	modelManager_ = ModelManager::GetInstance();
	modelManager_->Initialize();

	//LightManagerの初期化
	lightManager_ = LightManager::GetInstance();
	lightManager_->Initialize();

	//環境変数の読み込み
	GlobalVariables::GetInstance()->LoadFiles();

	//乱数生成機の初期化
	Random::Initialize();
}

void GameDirectionCenter::Finalize()
{
	//LightManagerの開放
	LightManager::Destroy();

	//ModelManagerの解放
	ModelManager::Destroy();

	//PostEffectsの解放
	PostEffects::Destroy();

	//ParticleManagerの開放
	ParticleManager::Destroy();

	//Audioの開放
	audio_->Finalize();

	//Inputの開放
	Input::Destroy();

	//GameObjectManagerの開放
	GameObjectManager::Destroy();

	//SceneManagerの開放
	SceneManager::Destroy();

	//Rendererの解放
	Renderer::Destroy();

	//ImGuiの解放
	imguiManager_->ShutDown();

	//TextureManagerの解放
	TextureManager::Destroy();

	//GraphicsCoreの解放
	GraphicsDirectionCenter::Destroy();

	//ゲームウィンドウを閉じる
	windowAppli_->CloseGameWindow();
}

void GameDirectionCenter::Update()
{
	//Inputの更新
	input_->Update();

	//ImGui受け付け開始
	imguiManager_->Begin();

	//ロードシーンに切り替え
	if (sceneManager_->GetLoadingScreenVisible())
	{
		isLoading_ = true;
	}

	//SceneManagerの更新
	sceneManager_->Update();

	//GlovalVariablesの更新
	GlobalVariables::GetInstance()->Update();

	//LightManagerの更新
	lightManager_->Update();

	//PostEffectsの更新
	postEffects_->Update();

	//ImGui受付終了
	imguiManager_->End();
}

void GameDirectionCenter::Draw()
{
	//描画前処理
	renderer_->PreDraw();

	//SceneManagerの描画
	sceneManager_->Draw();

	//描画後処理
	renderer_->PostDraw();

	//PostEffectsの適用
	postEffects_->Apply();

	//描画前処理
	graphicsCore_->PreDraw();

	//PostEffectsの描画
	postEffects_->Draw();

	//SceneManagerのスプライト描画
	sceneManager_->DrawUI();

	//ImGuiの描画
	imguiManager_->Draw();

	//描画後処理
	graphicsCore_->PostDraw();
}

bool GameDirectionCenter::IsEndRequest()
{
	bool endRequest = windowAppli_->ProcessMessage();
	return endRequest;
}

void GameDirectionCenter::Run()
{
	//初期化
	Initialize();

	bool exit = false;
	//バックグラウンドループ
	std::thread th([&]() {
		while (!exit)
		{
			std::unique_lock<std::mutex> uniqueLock(mutex);
			condition.wait(uniqueLock, [&]() {return true; });
			if (isLoading_)
			{
				sceneManager_->Load();
				isLoading_ = false;
			}
		}
	});


	//ゲームループ
	while (true)
	{
		//ゲームループを抜ける
		if (IsEndRequest())
		{
			break;
		}

		//更新
		Update();

		//描画
		Draw();
	}

	//終了
	exit = true;
	th.join();
	Finalize();
}