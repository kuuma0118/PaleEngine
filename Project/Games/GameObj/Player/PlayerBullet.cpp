#include "PlayerBullet.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Player.h"

void PlayerBullet::Initialize(const Vector3& position, const Vector3& velocity)
{
	//モデルの生成
	model_.reset(ModelManager::CreateFromModelFile("Cube.obj", Opaque));
	model_->GetMaterial()->SetEnableLighting(true);

	//速度の初期化
	velocity_ = velocity;

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = { 0.6f,0.6f,0.6f };

	AABB aabb = {
		.min{-worldTransform_.scale_.x,-worldTransform_.scale_.y,-worldTransform_.scale_.z,},
		.max{worldTransform_.scale_.x,worldTransform_.scale_.y,worldTransform_.scale_.z}
	};
	SetAABB(aabb);
	SetCollisionAttribute(kCollisionAttributeMissile);
	SetCollisionMask(kCollisionMaskMissile);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
}

void PlayerBullet::Update()
{
	//移動処理
	worldTransform_.translation_ += velocity_;

	//回転処理
	worldTransform_.rotation_.x += 0.2f;
	worldTransform_.rotation_.y += 0.2f;

	//ワールドトランスフォームの更新
	worldTransform_.UpdateMatrixFromEuler();

	//フィールド外に出たら死亡フラグを立てる
	if (worldTransform_.translation_.x <= -100.0f || worldTransform_.translation_.x >= 100.0f || worldTransform_.translation_.y <= 1.0f || worldTransform_.translation_.z <= -100.0f || worldTransform_.translation_.z >= 100.0f)
	{
		isDead_ = true;
	}
}