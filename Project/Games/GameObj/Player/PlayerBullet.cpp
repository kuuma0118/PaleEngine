#include "PlayerBullet.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Utility/GlobalVariables.h"

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
	SetCollisionAttribute(kCollisionAttributePlayerBullet);
	SetCollisionMask(kCollisionMaskPlayerBullet);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);

}

void PlayerBullet::Update()
{

	Move(worldTransform_.translation_, velocity_);

	/*if (--deathTimer_ <= 0) {
		isDead_ = true;
	}*/
	//ワールドトランスフォームの更新
	worldTransform_.UpdateMatrixFromEuler();

	//フィールド外に出たら死亡フラグを立てる
	//if (worldTransform_.translation_.x <= -100.0f || worldTransform_.translation_.x >= 100.0f || worldTransform_.translation_.y <= 100.0f || worldTransform_.translation_.z <= -100.0f || worldTransform_.translation_.z >= 100.0f)
	//{
	//	isDead_ = true;
	//}
}

void PlayerBullet::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, camera);
}
//
//void PlayerBullet::OnCollision(IGameObject* gameObject)
//{
//	//破壊フラグを立てる
//	isDead_ = true;
//}
//
//void PlayerBullet::OnCollisionEnter(IGameObject* gameObject)
//{
//}
//
//void PlayerBullet::OnCollisionExit(IGameObject* gameObject)
//{
//}