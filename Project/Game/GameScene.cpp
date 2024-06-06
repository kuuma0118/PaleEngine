#include "GameScene.h"
#include "GameManager.h"
#include "Components/PostProcess.h"
#include <cassert>

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

	//ポストプロセスの有効化
	PostProcess::GetInstance()->SetIsPostProcessActive(true);
	PostProcess::GetInstance()->SetIsBloomActive(true);

	viewProjection_.UpdateMatrix();
};

void GameScene::Update(GameManager* gameManager) {

	player_->Update();

	viewProjection_.UpdateMatrix();
};

void GameScene::Draw(GameManager* gameManager) {

	PostProcess::GetInstance()->PreDraw();

#pragma region 背景スプライトの描画

	//背景スプライトの描画
	Sprite::PreDraw(Sprite::kBlendModeNormal);

	Sprite::PostDraw();

#pragma endregion

#pragma region モデルの描画



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

	Sprite::PostDraw();

#pragma endregion

};