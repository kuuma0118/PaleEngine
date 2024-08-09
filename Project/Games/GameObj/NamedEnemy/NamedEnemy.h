#pragma once
#include "Engine/Framework/Object/IGameObject.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Engine/2D/Sprite.h"
#include "Games/GameObj/NamedEnemy/EnemyState/INamedEnemyState.h"
#include "Games/GameObj/NamedEnemy/EnemyState/StateMissileAttack.h"
#include "Games/GameObj/NamedEnemy/EnemyState/StateNormal.h"
#include "Missile.h"

class NamedEnemy : public IGameObject, public Collider
{
public:
	~NamedEnemy();

	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void DrawUI() override;

	void OnCollision(Collider* collider) override;

//	const Vector3 GetWorldPosition() const override;

//	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

	void ChangeState(INamedEnemyState* newState);

	const std::list<std::unique_ptr<Missile>>& GetMissiles() const { return missiles_; };

	void AddMissile(Missile* missile) { missiles_.push_back(std::unique_ptr<Missile>(missile)); };

	const bool GetIsAttack() const { return isAttack_; };

	void SetIsAttack(const bool isAttack) { isAttack_ = isAttack; };

	const float GetDamage() const { return damage_; };

	void SetDamage(const float damage) { damage_ = damage; };

	const float GetHP() const { return hp_; };

	Model* GetModel() { return model_; };

	void SetIsSlow(bool isSlow) { isSlow_ = isSlow; };

	const bool GetIsSlow() { return isSlow_; };

private:
	//状態
	INamedEnemyState* state_ = nullptr;

	//ミサイル
	std::list<std::unique_ptr<Missile>> missiles_{};

	//体力バー
	std::unique_ptr<Sprite> spriteHpBar_ = nullptr;
	std::unique_ptr<Sprite> spriteHpBarFrame_ = nullptr;

	//HP
	Vector2 hpBarSize_{ 480.0f,16.0f };
	const float kMaxHP = 800.0f;
	float hp_ = kMaxHP;

	//アクティブフラグ
	bool isActive_ = false;

	//攻撃フラグ
	bool isAttack_ = false;

	//ダメージ
	float damage_ = 0.0f;

	//当たり判定のフラグ
	bool onCollision_ = false;
	bool preOnCollision_ = false;

	//スロウ状態のフラグ
	static const int kSlowTime = 60 * 5;
	bool isSlow_ = false;
	int slowTimer_ = 0;
};

