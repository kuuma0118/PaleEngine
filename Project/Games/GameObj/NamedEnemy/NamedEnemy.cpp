#include "NamedEnemy.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Games/GameObj/Player/Player.h"

NamedEnemy::~NamedEnemy()
{
	delete state_;
}

void NamedEnemy::Initialize()
{
	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = { 0.0f,8.0f,100.0f };
	worldTransform_.rotation_ = { 0.0f,0.0f,0.0f };
	worldTransform_.scale_ = { 5.0f,5.0f,5.0f };

	//状態の初期化
	state_ = new NamedEnemyStateNormal();
	state_->Initialize(this);

	//体力バーのスプライトの生成
	//TextureManager::Load("HpBar.png");
	//spriteHpBar_.reset(Sprite::Create("HpBar.png", { 720.0f,32.0f }));
	//spriteHpBar_->SetColor({ 1.0f, 0.1f, 0.0f, 1.0f });

	//体力バーのフレームのスプライトの生成
	//TextureManager::Load("HpBarFrame.png");
	//spriteHpBarFrame_.reset(Sprite::Create("HpBarFrame.png", { 719.0f,31.0f }));
	//spriteHpBarFrame_->SetColor({ 1.0f, 0.1f, 0.0f, 1.0f });

	//衝突属性を設定
	AABB aabb{
	.min{-worldTransform_.scale_.x,-worldTransform_.scale_.y,-worldTransform_.scale_.z},
	.max{worldTransform_.scale_.x,worldTransform_.scale_.y,worldTransform_.scale_.z} };
	SetAABB(aabb);
	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(kCollisionMaskEnemy);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
}

void NamedEnemy::Update()
{
	////プレイヤーが動いたらボスも動き出す
	//if (!isActive_)
	//{
	//	if (GameObjectManager::GetInstance()->GetGameObject<Player>("Player")->GetVelocity() != Vector3{ 0.0f,0.0f,0.0f })
	//	{
	//		isActive_ = true;
	//	}
	//}

	isActive_ = true;

	//前のフレームの当たり判定のフラグを取得
	preOnCollision_ = onCollision_;
	onCollision_ = false;

	//状態の更新
	if (isActive_)
	{
		if (isSlow_)
		{
			if (++slowTimer_ > kSlowTime)
			{
				isSlow_ = false;
				slowTimer_ = 0;
			}
		}
		else
		{
			state_->Update(this);
		}
	}

	//死亡フラグの立ったミサイルを削除
	missiles_.remove_if([](std::unique_ptr<Missile>& missile)
		{
			if (missile->GetIsDead())
			{
				missile.reset();
				return true;
			}
			return false;
		}
	);

	//ミサイルの更新
	for (const std::unique_ptr<Missile>& missile : missiles_)
	{
		missile->Update();
	}

	
	//ワールドトランスフォームの更新
	worldTransform_ = state_->GetWorldTransform();
	worldTransform_.UpdateMatrixFromQuaternion();
	model_->Update(worldTransform_, 0);

	//アニメーションを再生
	if (!model_->GetAnimation()->IsPlaying())
	{
		animationNumber_ = 0;
		model_->GetAnimation()->PlayAnimation();
	}

	////HPバーの処理
	//hpBarSize_ = { (hp_ / kMaxHP) * 480.0f,16.0f };
	//spriteHpBar_->SetSize(hpBarSize_);
}

void NamedEnemy::Draw(const Camera& camera)
{
	//モデルの描画
	model_->Draw(worldTransform_, camera);

	//状態の描画
	state_->Draw(this, camera);

	//ミサイルの描画
	for (const std::unique_ptr<Missile>& missile : missiles_)
	{
		missile->Draw(camera);
	}

}

void NamedEnemy::DrawUI()
{
	spriteHpBar_->Draw();
	spriteHpBarFrame_->Draw();
}

void NamedEnemy::OnCollision(Collider* collider)
{
	if (collider->GetCollisionAttribute() == kCollisionAttributeWeapon)
	{
		onCollision_ = true;

		if (onCollision_ != preOnCollision_)
		{
			const Player* player = GameObjectManager::GetInstance()->GetGameObject<Player>("Player");
			hp_ -= player->GetDamage();
			if (hp_ <= 0.0f)
			{
				hp_ = 0.0f;
			}
			//衝突判定の応答
			state_->OnCollision(collider);
			worldTransform_ = state_->GetWorldTransform();
			worldTransform_.UpdateMatrixFromQuaternion();
		}
	}
}

const Vector3 NamedEnemy::GetWorldPosition() const
{
	Vector3 pos{};
	pos.x = worldTransform_.matWorld_.m[3][0];
	pos.y = worldTransform_.matWorld_.m[3][1];
	pos.z = worldTransform_.matWorld_.m[3][2];
	return pos;
}

void NamedEnemy::SetParent(const WorldTransform* parent)
{

	worldTransform_.parent_ = parent;

}

void NamedEnemy::ChangeState(INamedEnemyState* newState)
{
	delete state_;
	state_ = newState;
}