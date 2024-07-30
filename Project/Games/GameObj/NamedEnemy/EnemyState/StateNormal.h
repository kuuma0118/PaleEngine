#pragma once
#include "INamedEnemyState.h"
#include "Engine/Math/MathFunction.h"

class NamedEnemyStateNormal : public INamedEnemyState
{
public:
	//攻撃までの最小時間
	static const uint32_t kMinAttackInterval = 60 * 2;
	//攻撃までの最大時間
	static const uint32_t kMaxAttackInterval = 60 * 4;

	void Initialize(NamedEnemy* pBoss) override;

	void Update(NamedEnemy* pBoss) override;

	void Draw(NamedEnemy* pBoss, const Camera& camera) override;

	void OnCollision(Collider* collider) override;

	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

private:
	//ワールドトランスフォーム
	WorldTransform worldTransform_{};

	//クォータニオン
	Quaternion destinationQuaternion_{ 0.0f,0.0f,0.0f,1.0f };

	//ノックバック時の速度
	Vector3 knockBackVelocity_{};

	//ノックバックのフラグ
	bool isKnockBack_ = false;

	//ノックバックのタイマー
	uint32_t knockBackTimer_ = 0;
	uint32_t knockBackTime_ = 20;

	//攻撃用のタイマー
	uint32_t attackTimer_ = 0;
	uint32_t attackTime_ = 0;
};

