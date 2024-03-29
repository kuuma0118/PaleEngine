#include "Player.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Utility/GlobalVariables.h"

Player::~Player()
{
}

void Player::Initialize()
{
  
	textureHandle_ = TextureManager::Load("Resources/uvChecker.png");

	//Player
	playerWorldTransform_.translation_.x = 2.0f;
	playerWorldTransform_.translation_.y = 0.0f;
	playerWorldTransform_.scale_ = { 0.8f,0.8f,0.8f };

	playerWorldTransform_.UpdateMatrix();

	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	
	NormalScale_ = { 1.0f,1.0f,1.0f };

	//パーティクルの初期化
	particleModel_.reset(ParticleModel::CreateFromOBJ("Resources/Particle", "Particle.obj"));
	particleSystem_ = std::make_unique<ParticleSystem>();
	particleSystem_->Initialize();

	//パーティクルの初期化
	particleModelB_.reset(ParticleModel::CreateFromOBJ("Resources/ParticleB", "ParticleB.obj"));
	particleSystemB_ = std::make_unique<ParticleSystem>();
	particleSystemB_->Initialize();

	////エミッターの作成
	//ParticleEmitter* particleEmitter = EmitterBuilder()
	//	.SetArea({ -1.0f,-1.0f,0.0f }, { 1.0f,-1.0f,0.0f })
	//	.SetAzimuth(0.0f, 0.0f)
	//	.SetColor({ 1.0f,1.0f,1.0f,0.5f }, { 1.0f,1.0f,1.0f,0.5f })
	//	.SetCount(10)
	//	.SetDeleteTime(10)
	//	.SetElevation(0.0f, 0.0f)
	//	.SetEmitterName("PlayerMove")
	//	.SetFrequency(0.001f)
	//	.SetLifeTime(1.0f, 1.0f)
	//	.SetParticleType(ParticleEmitter::ParticleType::kScale)
	//	.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
	//	.SetScale({ 0.1f,0.1f,0.1f }, { 0.1f,0.1f,0.1f })
	//	.SetTranslation(playerWorldTransform_.translation_)
	//	.SetVelocity({ 0.3f,0.3f,0.3f }, { 0.3f,0.3f,0.3f })
	//	.Build();
	//particleSystem_->AddParticleEmitter(particleEmitter);

	////エミッターの作成
	//ParticleEmitter* particleEmitterB = EmitterBuilder()
	//	.SetArea({ -1.0f,-1.0f,0.0f }, { 1.0f,-1.0f,0.0f })
	//	.SetAzimuth(0.0f, 0.0f)
	//	.SetColor({ 1.0f,1.0f,0.1f,1.0f }, { 1.0f,1.0f,0.1f,1.0f })
	//	.SetCount(10)
	//	.SetDeleteTime(10)
	//	.SetElevation(0.0f, 0.0f)
	//	.SetEmitterName("PlayerMoveB")
	//	.SetFrequency(0.001f)
	//	.SetLifeTime(1.0f, 2.0f)
	//	.SetParticleType(ParticleEmitter::ParticleType::kNormal)
	//	.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
	//	.SetScale({ 0.03f,0.03f,0.03f }, { 0.1f,0.1f,0.1f })
	//	.SetTranslation(playerWorldTransform_.translation_)
	//	.SetVelocity({ 0.01f,0.01f,0.01f }, { 0.03f,0.03f,0.03f })
	//	.Build();
	//particleSystemB_->AddParticleEmitter(particleEmitterB);
}

void Player::Update() 
{
	//エミッターが消えていたら再生成
	if (particleSystem_->GetParticleEmitter("PlayerMove") == nullptr) {
		//エミッターの作成
		ParticleEmitter* particleEmitter = EmitterBuilder()
			.SetArea({ -1.0f,-1.0f,0.0f }, { 1.0f,-1.0f,0.0f })
			.SetAzimuth(0.0f, 0.0f)
			.SetColor({ 1.0f,1.0f,1.0f,0.5f }, { 1.0f,1.0f,1.0f,0.5f })
			.SetCount(10)
			.SetDeleteTime(1)
			.SetElevation(0.0f, 0.0f)
			.SetEmitterName("PlayerMove")
			.SetFrequency(0.001f)
			.SetLifeTime(1.0f, 1.0f)
			.SetParticleType(ParticleEmitter::ParticleType::kScale)
			.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetScale({ 0.1f,0.1f,0.1f }, { 0.1f,0.1f,0.1f })
			.SetTranslation(playerWorldTransform_.translation_)
			.SetVelocity({ 0.3f,0.3f,0.3f }, { 0.3f,0.3f,0.3f })
			.Build();
		particleSystem_->AddParticleEmitter(particleEmitter);
	}

	//エミッターが消えていたら再生成
	if (particleSystemB_->GetParticleEmitter("PlayerMoveB") == nullptr) {
		//エミッターの作成
		ParticleEmitter* particleEmitterB = EmitterBuilder()
			.SetArea({ -1.0f,-1.0f,0.0f }, { 1.0f,-1.0f,0.0f })
			.SetAzimuth(0.0f, 0.0f)
			.SetColor({ 1.0f,1.0f,0.1f,1.0f }, { 1.0f,1.0f,0.1f,1.0f })
			.SetCount(10)
			.SetDeleteTime(10)
			.SetElevation(0.0f, 0.0f)
			.SetEmitterName("PlayerMoveB")
			.SetFrequency(0.001f)
			.SetLifeTime(1.0f, 1.0f)
			.SetParticleType(ParticleEmitter::ParticleType::kNormal)
			.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetScale({ 0.1f,0.1f,0.1f }, { 0.1f,0.1f,0.1f })
			.SetTranslation(playerWorldTransform_.translation_)
			.SetVelocity({ 0.03f,0.03f,0.03f }, { 0.03f,0.03f,0.03f })
			.Build();
		particleSystemB_->AddParticleEmitter(particleEmitterB);
	}

				particleSystem_->GetParticleEmitter("PlayerMove")->SetPopArea({ 2.0f,-0.5f,0.0f }, { 2.0f,-0.5f,0.0f });
				particleSystem_->GetParticleEmitter("PlayerMove")->SetPopAzimuth(0.0f, 360.0f);
				particleSystem_->GetParticleEmitter("PlayerMove")->SetPopCount(36);
				particleSystem_->GetParticleEmitter("PlayerMove")->SetTranslation(playerWorldTransform_.translation_);
			
				particleSystemB_->GetParticleEmitter("PlayerMoveB")->SetPopArea({ -3.5f,-0.5f,0.0f }, { -3.5f,-0.5f,0.0f });
				particleSystemB_->GetParticleEmitter("PlayerMoveB")->SetPopAzimuth(0.0f, 360.0f);
				particleSystemB_->GetParticleEmitter("PlayerMoveB")->SetPopCount(36);
				particleSystemB_->GetParticleEmitter("PlayerMoveB")->SetTranslation(playerWorldTransform_.translation_);

			playerWorldTransform_.UpdateMatrix();
	prePlayerTranslation_ = playerWorldTransform_.translation_;
	Player::ApplyGlobalVariables();

	//パーティクルの更新
	particleSystem_->Update();
	particleSystemB_->Update();
}

void Player::Draw(const ViewProjection viewProjection)
{

}

void Player::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
}

void Player::DrawSprite()
{
	
}

void Player::DrawParticle(const ViewProjection& viewProjection) {
	//パーティクルモデルの描画
	particleModel_->Draw(particleSystem_.get(), viewProjection);
	particleModelB_->Draw(particleSystemB_.get(), viewProjection,textureHandle_);
}
