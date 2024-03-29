#include "GameTitleScene.h"
#include "GameManager.h"
#include "GameScene.h"
#include "Components/PostProcess.h"
#include <cassert>
#include <algorithm>

GameTitleScene::GameTitleScene() {};

GameTitleScene::~GameTitleScene() {};

void GameTitleScene::Initialize(GameManager* gameManager)
{
	//TextureManagerのインスタンスを取得
	textureManager_ = TextureManager::GetInstance();
	//Audioのインスタンスを取得
	audio_ = Audio::GetInstance();
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	viewProjection_.UpdateMatrix();

	//スプライトの生成

	titleTextureHandle_ = TextureManager::Load("Resources/Images/title.png");
	titleSprite_.reset(Sprite::Create(titleTextureHandle_, { 0.0f,0.0f }));

	pressATextureHandle_ = TextureManager::Load("Resources/Images/PressAbutton.png");
	pressASprite_.reset(Sprite::Create(pressATextureHandle_,
		{ WinApp::GetInstance()->kClientWidth * 0.5f - 508.0f * 0.5f , 550.0f }));

	//スプライトの生成
	transitionSprite_.reset(Sprite::Create(transitionTextureHandle_, { 0.0f,0.0f }));
	transitionSprite_->SetColor(transitionColor_);
	transitionSprite_->SetSize(Vector2{ 640.0f,360.0f });

	//ポストプロセスの有効化
	PostProcess::GetInstance()->SetIsPostProcessActive(true);
	PostProcess::GetInstance()->SetIsBloomActive(true);

};

void GameTitleScene::Update(GameManager* gameManager)
{
	Input::GetInstance()->GetJoystickState(joyState_);

	if (playerWorldTransform_.translation_.x <= -0.0f)
	{
		playerMoveSpeed_ = -0.0f;
		weaponMoveSpeed_ = -0.0f;
	}

	if (playerWorldTransform_.translation_.x >= 1.6f)
	{
		playerMoveSpeed_ = 0.0f;
		weaponMoveSpeed_ = 0.0f;
	}


	if (joyState_.Gamepad.wButtons & XINPUT_GAMEPAD_A)
	{
		if (isTransitionEnd_) {
			isTransition_ = true;
		}
	}

	if (isTransitionEnd_ == false) {
		transitionTimer_ += 1.0f / kTransitionTime;
		transitionColor_.w = Lerp(transitionColor_.w, 0.0f, transitionTimer_);
		transitionSprite_->SetColor(transitionColor_);

		if (transitionColor_.w <= 0.0f) {
			isTransitionEnd_ = true;
			transitionTimer_ = 0.0f;
		}
	}

	if (isTransition_) {
		transitionTimer_ += 1.0f / kTransitionTime;
		transitionColor_.w = Lerp(transitionColor_.w, 1.0f, transitionTimer_);
		transitionSprite_->SetColor(transitionColor_);

		if (transitionColor_.w >= 1.0f) {
			gameManager->ChangeScene(new GameScene);

		}
	}

	viewProjection_.UpdateMatrix();
};

void GameTitleScene::Draw(GameManager* gameManager)
{
	//背景スプライトの描画
	Sprite::PreDraw(Sprite::kBlendModeNormal);

	Sprite::PostDraw();

	DirectXCommon::GetInstance()->ClearDepthBuffer();

	PostProcess::GetInstance()->PreDraw();

	//モデルの描画
	Model::PreDraw();

	Model::PostDraw();

	//パーティクルの描画
	ParticleModel::PreDraw();

	ParticleModel::PostDraw();

	PostProcess::GetInstance()->PostDraw();

	//スプライトの描画処理
	Sprite::PreDraw(Sprite::kBlendModeNormal);


	titleSprite_->Draw();

	pressASprite_->Draw();

	transitionSprite_->Draw();

	Sprite::PostDraw();
};