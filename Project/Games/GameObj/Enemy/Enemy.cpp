#include "Enemy.h"
#include "Games/GameObj/Player/Player.h"
#include "Engine/Framework/Object/GameObjectManager.h"

Enemy::~Enemy() {
	for (TimedCall* timedCall : timedCalls_) {
		delete timedCall;
	}
	for (std::unique_ptr<EnemyBullet>& bullet : bullet_) {
		bullet.release();
	}
	delete state_;
}

void Enemy::Initialize() {

	worldTransform_.Initialize();

	worldTransform_.scale_ = { 2.0f,2.0f,2.0f };

	state_ = new EnemyStateApproach();
	state_->Initialize(this);
	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributeEnemy);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(~kCollisionAttributeEnemy);
	SetRadius(1.0f);
}

void Enemy::ChangeState(IEnemyState* newState) { 
	state_ = nullptr;
	state_ = newState;
}

void Enemy::EnemyMove(Vector3 move) {
	Move(worldTransform_.translation_, move);
}

void Enemy::OnCollision(Collider* collider)
{
	isDead_ = true;
}

void Enemy::Update() {
	// 弾の削除
	timedCalls_.remove_if([](TimedCall* timedCall) {
		if (timedCall->IsFinished() == true) {
			delete timedCall;
			return true;
		}
		return false;
	});

	state_->Update(this);

	//行列の更新
	worldTransform_.UpdateMatrixFromEuler();
	
}

void Enemy::Fire() {
	//弾の速度
	const float kBulletSpeed = 0.5f;

	//敵キャラのワールド座標を取得する
	Vector3 enemyPos = worldTransform_.translation_;
	Vector3 velocity = {0,0,kBulletSpeed};

	//弾の生成
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(worldTransform_.translation_,velocity);

	//弾を登録する
	bullet_.push_back(std::unique_ptr<EnemyBullet>(newBullet));
}

void Enemy::FireReset() {
	Fire();
	// メンバ関数と呼び出し元をbindしてstd::functionに代入
	std::function<void(void)> callback = std::bind(&Enemy::FireReset, this);
	//時限発動イベントを生成
	TimedCall* timedCall = new TimedCall(callback, 60);
	//時限発動イベントを時限発動イベントリストに追加
	timedCalls_.push_back(timedCall);
}

void Enemy::Draw(const Camera& camera) {
	model_->Draw(worldTransform_, camera);
	//bulletの描画
	//for (const std::unique_ptr<EnemyBullet>& bullets : bullet_)
	//{
	//	bullets->Draw(camera);
	//}
}

void Enemy::DrawUI()
{
}

const Vector3 Enemy::GetWorldPosition() const
{
	Vector3 pos{};
	pos.x = worldTransform_.matWorld_.m[3][0];
	pos.y = worldTransform_.matWorld_.m[3][1];
	pos.z = worldTransform_.matWorld_.m[3][2];
	return pos;
}