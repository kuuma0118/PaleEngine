#include "Player.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Utility/GlobalVariables.h"

Player::~Player()
{
}

void Player::Initialize()
{
  
	textureHandle_ = TextureManager::Load("Resources/monsterBall.png");

	// モデルの読み込み
	model_.reset(Model::CreateFromOBJ("Resources/Sphere", "Sphere.obj"));
	assert(model_);

	//Player
	playerWorldTransform_.translation_.x = 0.0f;
	playerWorldTransform_.translation_.y = 0.0f;
	playerWorldTransform_.scale_ = { 1.0f,1.0f,1.0f };

	playerWorldTransform_.UpdateMatrix();

	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	
	NormalScale_ = { 1.0f,1.0f,1.0f };

}

void Player::Update() 
{
			
			playerWorldTransform_.UpdateMatrix();
	prePlayerTranslation_ = playerWorldTransform_.translation_;
	Player::ApplyGlobalVariables();

}

void Player::Draw(const ViewProjection viewProjection)
{
	model_->Draw(playerWorldTransform_, viewProjection, textureHandle_);
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

}
