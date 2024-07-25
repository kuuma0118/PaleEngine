#pragma once
#include "Engine/Math/WorldTransform.h"
#include "Engine/Framework/Object/IGameObject.h"
#include "Engine/Components/Collision/Collider.h"

class Player;

class EnemyBullet : public Collider{
public:
	static const int32_t kLifeTime = 30;
	void Initialize(const Vector3& position, const Vector3& velocity);
	void OnCollision(Collider* collider) override;
	const Vector3 GetWorldPosition() const override;
	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };
	void Update();
	void Draw(const Camera& viewProjection);
	void SetPlayer(Player* player) { player_ = player; };
	bool isDead() const { return isDead_; };

private:
	WorldTransform worldTransform_;
	//モデル
	std::unique_ptr<Model> model_ = nullptr;
	uint32_t textureHandle_ = 0u;
	Vector3 velocity_;
	Player* player_;
	float t_ = 0.0f;
	bool isDead_ = false;
};