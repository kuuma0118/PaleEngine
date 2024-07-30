#include "StateMissileAttack.h"
#include "../NamedEnemy.h"

void NamedEnemyStateMissileAttack::Initialize(NamedEnemy* pBoss)
{
	worldTransform_ = pBoss->GetWorldTransform();
	destinationQuaternion_ = worldTransform_.quaternion_;
}

void NamedEnemyStateMissileAttack::Update(NamedEnemy* pBoss)
{
	//待機処理
	if (!isAttack_)
	{
		if (++waitTimer_ > kWaitTime)
		{
			isAttack_ = true;
		}
	}

	//攻撃処理
	if (isAttack_ && !isRecovery_)
	{
		if (++fireTimer_ > kFireInterval)
		{
			fireTimer_ = 0;
			fireCount_++;
			Vector3 velocity[4];
			velocity[0] = { 0.2f,0.2f,0.0f };
			velocity[0] = Mathseries::TransformNormal(velocity[0], worldTransform_.matWorld_);
			velocity[1] = { -0.2f,0.2f,0.0f };
			velocity[1] = Mathseries::TransformNormal(velocity[1], worldTransform_.matWorld_);

			Vector3 translation = pBoss->GetWorldPosition();
			Missile* newMissile1 = new Missile();
			newMissile1->Initialize(translation, velocity[0]);
			pBoss->AddMissile(newMissile1);

			Missile* newMissile2 = new Missile();
			newMissile2->Initialize(translation, velocity[1]);
			pBoss->AddMissile(newMissile2);

		}

		if (fireCount_ >= 3)
		{
			isRecovery_ = true;
		}
	}

	//硬直処理
	if (isRecovery_)
	{
		if (++recoveryTimer_ > kRecoveryTime)
		{
			INamedEnemyState* newState = new NamedEnemyStateNormal();
			newState->Initialize(pBoss);
			pBoss->ChangeState(newState);
		}
	}

	//移動限界座標
	const float kMoveLimitX = 47;
	const float kMoveLimitZ = 47;
	worldTransform_.translation_.x = std::max<float>(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = std::min<float>(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.z = std::max<float>(worldTransform_.translation_.z, -kMoveLimitZ);
	worldTransform_.translation_.z = std::min<float>(worldTransform_.translation_.z, +kMoveLimitZ);
}

void NamedEnemyStateMissileAttack::Draw(NamedEnemy* pBoss, const Camera& camera)
{

}

void NamedEnemyStateMissileAttack::OnCollision(Collider* collider)
{
	if (collider->GetCollisionAttribute() == kCollisionAttributeWeapon)
	{

	}
}