#include "StateMissileAttack.h"
#include "../NamedEnemy.h"

void NamedEnemyStateMissileAttack::Initialize(NamedEnemy* pBoss)
{
	worldTransform_ = pBoss->GetWorldTransform();
	destinationQuaternion_ = worldTransform_.quaternion_;
}

void NamedEnemyStateMissileAttack::Update(NamedEnemy* Named)
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
			Vector3 velocity[12];
			velocity[0] = { -0.2f,0.2f,0.0f };
			velocity[0] = Mathseries::TransformNormal(velocity[0], worldTransform_.matWorld_);
			velocity[1] = { -0.2f,0.0f,0.0f };
			velocity[1] = Mathseries::TransformNormal(velocity[1], worldTransform_.matWorld_);
			velocity[2] = { -0.4f,-0.2f,0.0f };
			velocity[2] = Mathseries::TransformNormal(velocity[2], worldTransform_.matWorld_);
			velocity[3] = { 0.2f,0.2f,0.0f };
			velocity[3] = Mathseries::TransformNormal(velocity[3], worldTransform_.matWorld_);
			velocity[4] = { 0.2f,0.0f,0.0f };
			velocity[4] = Mathseries::TransformNormal(velocity[4], worldTransform_.matWorld_);
			velocity[5] = { 0.4f,0.2f,0.0f };
			velocity[5] = Mathseries::TransformNormal(velocity[5], worldTransform_.matWorld_);
			velocity[6] = { 0.0f,0.4f,0.0f };
			velocity[6] = Mathseries::TransformNormal(velocity[6], worldTransform_.matWorld_);
			velocity[7] = { 0.0f,-0.4f,0.0f };
			velocity[7] = Mathseries::TransformNormal(velocity[7], worldTransform_.matWorld_);

			Vector3 translation = Named->GetWorldPosition();
			Missile* newMissile1 = new Missile();
			newMissile1->Initialize(translation, velocity[0]);
			Named->AddMissile(newMissile1);

			Missile* newMissile2 = new Missile();
			newMissile2->Initialize(translation, velocity[1]);
			Named->AddMissile(newMissile2);

			Missile* newMissile3 = new Missile();
			newMissile3->Initialize(translation, velocity[2]);
			Named->AddMissile(newMissile3);

			Missile* newMissile4 = new Missile();
			newMissile4->Initialize(translation, velocity[3]);
			Named->AddMissile(newMissile4);

			Missile* newMissile5 = new Missile();
			newMissile5->Initialize(translation, velocity[4]);
			Named->AddMissile(newMissile5);

			Missile* newMissile6 = new Missile();
			newMissile6->Initialize(translation, velocity[5]);
			Named->AddMissile(newMissile6);

			Missile* newMissile7 = new Missile();
			newMissile7->Initialize(translation, velocity[6]);
			Named->AddMissile(newMissile7);

			Missile* newMissile8 = new Missile();
			newMissile8->Initialize(translation, velocity[7]);
			Named->AddMissile(newMissile8);


		}

		if (fireCount_ >= 8)
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
			newState->Initialize(Named);
			Named->ChangeState(newState);
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