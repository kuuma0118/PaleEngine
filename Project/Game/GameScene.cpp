#include "GameScene.h"
#include "GameTitleScene.h"
#include "GameManager.h"
#include "Components/PostProcess.h"
#include <cassert>
#include <algorithm>

GameScene::GameScene() {};

GameScene::~GameScene() {
	delete debugCamera_;
};

void GameScene::Initialize(GameManager* gameManager) {

	//TextureManagerのインスタンスを取得
	textureManager_ = TextureManager::GetInstance();
	//Audioのインスタンスを取得
	audio_ = Audio::GetInstance();
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();
	//デバッグカメラの生成
	debugCamera_ = new DebugCamera();
	pressATextureHandle_ = TextureManager::Load("Resources/Images/PressAbutton.png");
	pressASprite_.reset(Sprite::Create(pressATextureHandle_,
		{ WinApp::GetInstance()->kClientWidth * 0.5f - 508.0f * 0.5f , 550.0f }));
	// 自キャラの生成
	player_ = std::make_unique<Player>();
	// 自キャラの初期化
	// 自キャラの初期化
	player_->Initialize();

	//ポストプロセスの有効化
	PostProcess::GetInstance()->SetIsPostProcessActive(true);
	PostProcess::GetInstance()->SetIsBloomActive(true);

	viewProjection_.UpdateMatrix();
};

void GameScene::Update(GameManager* gameManager) {

	Input::GetInstance()->GetJoystickState(joyState_);

	player_->Update();

	if (joyState_.Gamepad.wButtons & XINPUT_GAMEPAD_A)
	{
		gameManager->ChangeScene(new GameTitleScene);
	}
	ImGui::Begin("light");
	ImGui::DragFloat3("translation", &viewProjection_.translation_.x, 0.001f, -100, 100);
	ImGui::End();

	viewProjection_.UpdateMatrix();
};

void GameScene::Draw(GameManager* gameManager) {

	PostProcess::GetInstance()->PreDraw();

#pragma region 背景スプライトの描画

	//背景スプライトの描画
	Sprite::PreDraw(Sprite::kBlendModeNormal);

	Sprite::PostDraw();

#pragma endregion

	//深度バッファをクリア
	DirectXCommon::GetInstance()->ClearDepthBuffer();

#pragma region モデルの描画

	//モデルの描画
	Model::PreDraw();

	player_->Draw(viewProjection_);

	Model::PostDraw();

#pragma endregion

#pragma region パーティクルの描画

	//パーティクルモデルの描画
	ParticleModel::PreDraw();

	player_->DrawParticle(viewProjection_);

	ParticleModel::PostDraw();

#pragma endregion

	PostProcess::GetInstance()->PostDraw();

#pragma region 前景スプライトの描画

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	player_->DrawSprite();

	pressASprite_->Draw();

	Sprite::PostDraw();

#pragma endregion

};