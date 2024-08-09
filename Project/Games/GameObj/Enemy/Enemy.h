#pragma once
#include "Engine/Math/WorldTransform.h"
#include "Engine/Framework/Object/IGameObject.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "EnemyBullet.h"
#include "TimeCall.h"
#include <optional>
#include "IEnemyState.h"
#include "EnemyStateApproach.h"
#include "EnemyStateLeave.h"


class Enemy : public IGameObject, public Collider{
public:
	static const int kFireInterval = 60;
	~Enemy();
	void Initialize() override;
	void Update() override;
	void OnCollision(Collider* collider) override;
	//const Vector3 GetWorldPosition() const override;
	void Fire();
	void FireReset();
	void Draw(const Camera& viewProjection) override;
	void DrawUI()override;
	void ChangeState(IEnemyState* newState);
	//const WorldTransform& GetWorldTransform() const override { return worldTransform_; };
	void EnemyMove(Vector3 move);
	int32_t GetFireTimer() { return fireTimer_; };
	void SetFireTimer(int32_t fireTimer) { this->fireTimer_ = fireTimer; };
	std::list<TimedCall*> GetTimedCall() { return timedCalls_; };
	const bool isDead() const { return isDead_; };

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	//モデル
	std::unique_ptr<Model> model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	//statePattern
	IEnemyState* state_;
	////弾
	//発射タイマー
	int32_t fireTimer_ = kFireInterval;
	//時限発動のリスト
	std::list<TimedCall*> timedCalls_;
	//生存フラグ
	bool isDead_ = false;

	//エネミー弾
	std::list<std::unique_ptr<EnemyBullet>> bullet_{};
};