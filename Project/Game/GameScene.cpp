#include "GameScene.h"
#include "GameManager.h"
#include "GameTitleScene.h"
#include "GameClearScene.h"
#include "GameOverScene.h"
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

	// 自キャラの生成
	choco_ = std::make_unique<chocolate>();
	// 自キャラの初期化
 	choco_->Initialize();
  
	// 自キャラの生成
	player_ = std::make_unique<Player>();
	// 自キャラの初期化
	player_->Initialize(choco_.get());
  
	//ボスの作成
	redbox_ = std::make_unique<RedBox>();
	redbox_->StartAnimationInit();
	redbox_->SetWeapon(choco_.get());

	//衝突マネージャーの生成
	collisionManager_ = std::make_unique<CollisionManager>();

	//背景の生成
	backGround_ = std::make_unique<BackGround>();
	backGround_->Initialize();

	//4x3用スプライトの生成
	x4x3TextureHandle_ = TextureManager::Load("Resources/Images/4x3Black.png") ;
	x4x3Sprite_.reset(Sprite::Create(x4x3TextureHandle_, { 0.0f,0.0f }));

	//トランジション用スプライトの生成
	transitionSprite_.reset(Sprite::Create(transitionTextureHandle_, { 0.0f,0.0f }));
	transitionSprite_->SetColor(transitionColor_);
	transitionSprite_->SetSize(Vector2{ 640.0f,360.0f });

	soundHandle_ = audio_->SoundLoadWave("Resources/Sounds/GameScene.wav");
	audio_->SoundPlayWave(soundHandle_, true);

	//ポストプロセスの有効化
	PostProcess::GetInstance()->SetIsPostProcessActive(true);
	PostProcess::GetInstance()->SetIsBloomActive(true);

	viewProjection_.UpdateMatrix();
};

void GameScene::Update(GameManager* gameManager) {

	//トランジション
	if (isTransitionEnd_ == false) {
		transitionTimer_ += 1.0f / kTransitionTime;
		transitionColor_.w = Lerp(transitionColor_.w, 0.0f, transitionTimer_);
		transitionSprite_->SetColor(transitionColor_);

		if (transitionColor_.w <= 0.0f) {
			isTransitionEnd_ = true;
			transitionTimer_ = 0.0f;
		}
	}

	if (isAnimationEnd_ == false) {
		//プレイヤーのアニメーションの更新
		player_->StartAnimation();

		//武器のアニメーションの更新
		choco_->StartAnimaion();

		//ボスのアニメーション更新
		redbox_->StartAnimation();

		//衝突判定
		collisionManager_->ClearColliderList();
		collisionManager_->SetColliderList(redbox_.get());
		collisionManager_->SetColliderList(choco_.get());
		collisionManager_->CheckAllCollisions();

		viewProjection_.UpdateMatrix();

		if (redbox_->GetAnimationEnd()) {
			isAnimationEnd_ = true;
		}

		return;
	}


	player_->Update();

	choco_->Update();
	choco_->GetBossIsDead(redbox_->GetIsDeadAnimation());

  	//ボスの更新
	redbox_->Update();


	//衝突判定
	collisionManager_->ClearColliderList();
	collisionManager_->SetColliderList(redbox_.get());
	const std::list<std::unique_ptr<Laser>>& lasers = redbox_->GetLaser();
	for (const std::unique_ptr<Laser>& laser : lasers) {
		collisionManager_->SetColliderList(laser.get());
	}
	/*const std::list<std::unique_ptr<Missile>>& missiles = boss_->GetMissiles();
	for (const std::unique_ptr<Missile>& missile : missiles) {
		collisionManager_->SetColliderList(missile.get());
	}*/
	collisionManager_->SetColliderList(choco_.get());
	//const std::list<std::unique_ptr<ChargeShot>>& chargeShots = boss_->GetChargeShot();
	//for (const std::unique_ptr<ChargeShot>& chargeShot : chargeShots) {
	//	collisionManager_->SetColliderList(chargeShot.get());
	//}
	collisionManager_->CheckAllCollisions();

	//背景の更新
	backGround_->Update();

	//デバッグカメラの更新
	debugCamera_->Update();
	//デバッグカメラの切り替え
	if (input_->IsPushKeyEnter(DIK_1)) {
		if (isDebugCameraActive_ == false) {
			isDebugCameraActive_ = true;
		}
		else {
			isDebugCameraActive_ = false;
		}
	}

	if (redbox_->GetIsDead())
	{
		if (isTransition_ == false && isTransitionEnd_ == true) {
			isTransition_ = true;
			audio_->StopAudio(soundHandle_);
			nextScene_ = NextScene::GAMECLEAR;
		}
	}

	if (choco_->GetHP() <= 0.0f)
	{
		if (isTransition_ == false && isTransitionEnd_ == true) {
			isTransition_ = true;
			audio_->StopAudio(soundHandle_);
			nextScene_ = NextScene::GAMEOVER;
		}
	}

	if (input_->IsPushKeyEnter(DIK_T))
	{
		if (isTransition_ == false && isTransitionEnd_ == true) {
			isTransition_ = true;
			audio_->StopAudio(soundHandle_);
			gameManager->ChangeScene(new GameTitleScene());
		}
	}

	if (isTransition_) {
		transitionTimer_ += 1.0f / kTransitionTime;
		transitionColor_.w = Lerp(transitionColor_.w, 1.0f, transitionTimer_);
		transitionSprite_->SetColor(transitionColor_);

		if (transitionColor_.w >= 1.0f) {
			switch (nextScene_) {
			case NextScene::GAMECLEAR:
				gameManager->ChangeScene(new GameClearScene());
				break;
			case NextScene::GAMEOVER:
				gameManager->ChangeScene(new GameOverScene());
				break;
			}
		}
	}

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

	choco_->Draw(viewProjection_);

	//ボスの描画
	redbox_->Draw(viewProjection_);

	//背景の描画
	backGround_->Draw(viewProjection_);

	Model::PostDraw();

#pragma endregion

#pragma region パーティクルの描画

	//パーティクルモデルの描画
	ParticleModel::PreDraw();

	player_->DrawParticle(viewProjection_);

	choco_->DrawParticle(viewProjection_);

	redbox_->DrawParticle(viewProjection_);

	ParticleModel::PostDraw();

#pragma endregion

	PostProcess::GetInstance()->PostDraw();

#pragma region 前景スプライトの描画

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	choco_->DrawSprite();

	redbox_->DrawSprite();

	player_->DrawSprite();

	x4x3Sprite_->Draw();
  
	transitionSprite_->Draw();
  
	Sprite::PostDraw();

#pragma endregion

};