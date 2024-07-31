#include "Missile.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Games/GameObj/Player/Player.h"
#include "Games/GameObj/NamedEnemy/NamedEnemy.h"

void Missile::Initialize(const Vector3& position, const Vector3& velocity)
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

void Missile::Update()
{
	//目標への差分ベクトルを計算
	Vector3 targetPosition = GameObjectManager::GetInstance()->GetGameObject<Player>("Player")->GetWorldPosition();
	Vector3 sub = targetPosition - worldTransform_.translation_;

	if (!isRepelled_)
	{
		//距離を計算
		float distance = Mathseries::Length(sub);

		//正規化
		sub = Mathseries::Normalize(sub);
		velocity_ = Mathseries::Normalize(velocity_);
	}

	//媒介変数の更新
	if (trackingParameter_ < 0.1f) {
		trackingParameter_ += 1.0f / (60.0f * 1.0f);
	}

	//追尾タイマーを進める
	if (trackingTimer_ != kTrackingTime)
	{
		trackingTimer_++;
	}

	if (trackingTimer_ <= kTrackingTime / 4.0f)
	{
		velocity_.z = 1.0f / 2.0f;
	}

	//一定時間追尾するようにする
	if (trackingTimer_ >= kTrackingTime / 2.0f)
	{
		isTrackingComplete_ = true;
	}

	if (trackingTimer_ >= kTrackingTime)
	{
		isDead_ = true;
	}

	//追捕終了していなかったら
	if (!isTrackingComplete_ && !isRepelled_)
	{
		velocity_ = Mathseries::Slerp(velocity_, sub, trackingParameter_);
		const float kSpeed = 0.6f;
		velocity_ *= kSpeed;
	}

	//移動処理
	worldTransform_.translation_ += velocity_;

	//回転処理
	/*worldTransform_.rotation_.x += 0.2f;
	worldTransform_.rotation_.y += 0.2f;*/

	//ワールドトランスフォームの更新
	worldTransform_.UpdateMatrixFromEuler();

	////フィールド外に出たら死亡フラグを立てる
	//if (worldTransform_.translation_.x <= -100.0f || worldTransform_.translation_.x >= 100.0f || worldTransform_.translation_.y <= 100.0f || worldTransform_.translation_.z <= -100.0f || worldTransform_.translation_.z >= 100.0f)
	//{
	//	isDead_ = true;
	//}
}

void Missile::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, camera);
}

void Missile::OnCollision(Collider* collider)
{
	if (collider->GetCollisionAttribute() == kCollisionAttributeWeapon && !isDead_)
	{
		SetCollisionAttribute(kCollisionAttributeWeapon);
		SetCollisionMask(kCollisionMaskWeapon);
		isRepelled_ = true;
		const float kSpeed = 0.6f;
		Vector3 targetPosition = GameObjectManager::GetInstance()->GetGameObject<NamedEnemy>("NamedEnemy")->GetWorldPosition();
		Vector3 sub = targetPosition - worldTransform_.translation_;
		sub = Mathseries::Normalize(sub);
		velocity_ = sub * kSpeed;
	}

	if (isRepelled_)
	{
		if (collider->GetCollisionAttribute() & kCollisionAttributeEnemy)
		{
			isDead_ = true;
		}
	}
	else
	{
		if (collider->GetCollisionAttribute() & kCollisionAttributePlayer)
		{
			isDead_ = true;
		}
	}
}

const Vector3 Missile::GetWorldPosition() const
{
	Vector3 pos{};
	pos.x = worldTransform_.matWorld_.m[3][0];
	pos.y = worldTransform_.matWorld_.m[3][1];
	pos.z = worldTransform_.matWorld_.m[3][2];
	return pos;
}