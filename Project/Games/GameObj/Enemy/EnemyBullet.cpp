#include "EnemyBullet.h"
#include "Engine/Math/MathFunction.h"
#include <cassert>
#include "Games/GameObj/Player/Player.h"
#include "Engine/Components/Collision/Collider.h"

void EnemyBullet::Initialize(const Vector3& position,const Vector3& velocity) {
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
//
//void EnemyBullet::OnCollision(Collider* collider) {
//	isDead_ = true;
//}

void EnemyBullet::Update() {
	SetPlayer(player_);

	//敵弾から自キャラへのベクトルを計算
	Vector3 toPlayer = Subtract(player_->GetWorldPosition(), worldTransform_.translation_);

	if (t_ < 0.05) {
		t_ += 1.0f / 120.0f;
	}

	// ベクトルを正規化する
	Vector3 nPlayer = Mathseries::Normalize(toPlayer);
	Vector3 nVelocity = Mathseries::Normalize(velocity_);

	if (player_->GetWorldPosition().z <= EnemyBullet::GetWorldPosition().z) {
		velocity_ = Mathseries::Slerp(nVelocity, nPlayer, t_);
		velocity_.x *= 0.5f;
		velocity_.y *= 0.5f;
		velocity_.z *= 0.5f;
		worldTransform_.rotation_.y = std::atan2(velocity_.x, velocity_.z);
		float length = Mathseries::Length(Vector3{velocity_.x, 0, velocity_.z});
		worldTransform_.rotation_.x = std::atan2(-velocity_.y, length);
	}

	Move(worldTransform_.translation_, velocity_);
	worldTransform_.UpdateMatrixFromEuler();
}

void EnemyBullet::Draw(const Camera& camera) {
	model_->Draw(worldTransform_, camera);
}

//const Vector3 EnemyBullet::GetWorldPosition() const {
//	// ワールド座標を入れる変数
//	Vector3 worldPos;
//	// ワールド行列の平行移動成分を取得(ワールド座標)
//	worldPos.x = worldTransform_.matWorld_.m[3][0];
//	worldPos.y = worldTransform_.matWorld_.m[3][1];
//	worldPos.z = worldTransform_.matWorld_.m[3][2];
//	return worldPos;
//}