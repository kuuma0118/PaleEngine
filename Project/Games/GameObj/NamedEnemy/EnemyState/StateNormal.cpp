#include "StateNormal.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Utility/Random.h"
#include "../NamedEnemy.h"
#include "Games/GameObj/Player/Player.h"

void NamedEnemyStateNormal::Initialize(NamedEnemy* pBoss)
{
	worldTransform_ = pBoss->GetWorldTransform();
	destinationQuaternion_ = worldTransform_.quaternion_;
	attackTime_ = Random::GetRandomInt(kMinAttackInterval, kMaxAttackInterval);
}

void NamedEnemyStateNormal::Update(NamedEnemy* pBoss)
{
	////移動方向を計算
	//Vector3 targetPosition = GameObjectManager::GetInstance()->GetGameObject<Player>("Player")->GetWorldPosition();
	//Vector3 sub = targetPosition - worldTransform_.translation_;
	//sub.y = 0.0f;

	////距離を計算
	//float distance = Mathseries::Length(sub);

	////正規化して移動量を掛ける
	//sub = Mathseries::Normalize(sub);
	//float kSpeed = 0.2f;
	//if (pBoss->GetIsSlow())
	//{
	//	kSpeed = 0.1f;
	//}

	////回転処理
	//Vector3 cross = Mathseries::Normalize(Mathseries::Cross({ 0.0f,0.0f,1.0f }, sub));
	//float dot = Mathseries::Dot({ 0.0f,0.0f,1.0f }, sub);
	//destinationQuaternion_ = Mathseries::Normalize(Mathseries::MakeRotateAxisAngleQuaternion(cross, std::acos(dot)));
	//worldTransform_.quaternion_ = Mathseries::Slerp(worldTransform_.quaternion_, destinationQuaternion_, 0.4f);

	////移動処理
	//if (distance >= 10.0f)
	//{
	//	worldTransform_.translation_ += sub * kSpeed;
	//}

	//攻撃処理
	if (++attackTimer_ > attackTime_ && !pBoss->GetIsSlow())
	{
		//uint32_t attackNum = 1;
		INamedEnemyState* newState = nullptr;
	/*	switch (attackNum)
		{

		case 0:*/
			newState = new NamedEnemyStateMissileAttack();
			newState->Initialize(pBoss);
		/*	break;
	
		}*/
		pBoss->ChangeState(newState);
	}

	//移動限界座標
	const float kMoveLimitX = 47;
	const float kMoveLimitZ = 47;
	worldTransform_.translation_.x = std::max<float>(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = std::min<float>(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.z = std::max<float>(worldTransform_.translation_.z, -kMoveLimitZ);
	worldTransform_.translation_.z = std::min<float>(worldTransform_.translation_.z, +kMoveLimitZ);
}

void NamedEnemyStateNormal::Draw(NamedEnemy* pBoss, const Camera& camera)
{

}

void NamedEnemyStateNormal::OnCollision(Collider* collider)
{
	if (collider->GetCollisionAttribute() == kCollisionAttributeWeapon)
	{
		//ノックバックの速度を決める
		const Player* player = GameObjectManager::GetInstance()->GetGameObject<Player>("Player");
		knockBackVelocity_ = player->GetVelocity();
		//if (player->GetComboIndex() == 3)
		//{
		//	Vector3 sub = worldTransform_.translation_ - player->GetWorldPosition();
		//	sub = Mathseries::Normalize(sub);
		//	sub.y = 0.0f;
		//	const float kKnockBackSpeed = 2.0f;
		//	knockBackVelocity_ = sub * kKnockBackSpeed;
		//}

		////ノックバックのフラグを立てる
		//isKnockBack_ = true;

		////ノックバックのタイマーを設定
		//knockBackTimer_ = 0;
		//knockBackTime_ = player->GetAttackTotalTime() - player->GetAttackParameter();
	}
}