#pragma once
#include "Engine/Framework/Object/IGameObject.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/2D/Sprite.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Math/WorldTransform.h"

/// <summary>
/// 自キャラの弾
/// </summary>
class PlayerBullet :  public Collider {
public: // メンバ関数

	//弾が消える時間
	static const int32_t kBulletTime = 300;

	void Initialize(const Vector3& position, const Vector3& velocity);

	void Update();

	void Draw(const Camera& camera);

	void OnCollision(Collider* collider) override;

	const Vector3 GetWorldPosition() const override;

	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

	const bool GetIsDead() const { return isDead_; };

	void SetTranslation(const Vector3& translation) { worldTransform_.translation_ = translation; };

private:
	//モデル
	std::unique_ptr<Model> model_ = nullptr;

	//ワールドトランスフォーム
	WorldTransform worldTransform_{};

	//速度
	Vector3 velocity_{};

	//媒介変数
	float trackingParameter_ = 0.0f;

	//デスタイマー
	int32_t deathTimer_ = kBulletTime;

	//死亡フラグ
	bool isDead_ = false;

	//オーディオハンドル
	uint32_t audioHandle_ = 0;

	//プレイヤーにはじかれたかどうか
	bool isRepelled_ = false;
};