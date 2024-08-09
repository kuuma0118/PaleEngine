#include "Player.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include <numbers>


Player::~Player() {
	for (std::unique_ptr<PlayerBullet>& bullet : bullet_) {
		bullet.release();
	}
}

void Player::Initialize()
{
	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_.z = 2.0f;
	worldTransform_.scale_ = {5.0f,5.0f,5.0f};
	
	//入力クラスのインスタンスを取得
	input_ = Input::GetInstance();

	//3Dレティクルのワールドトランスフォーム初期化
	worldTransform3DReticle_.Initialize();

	TextureManager::Load("Raticle.png");
	spriteRaticle_.reset(Sprite::Create("Raticle.png", { 640.0f,360.0f }));
	spriteRaticle_->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f });
	spriteRaticle_->SetPosition(Vector2((float)WindowAppli::kClientWidth / 2, (float)WindowAppli::kClientHeight / 2));

}

void Player::Update()
{

	//死亡フラグの立ったミサイルを削除
	bullet_.remove_if([](std::unique_ptr<PlayerBullet>& bullet)
		{
			if (bullet->GetIsDead())
			{
				bullet.reset();
				return true;
			}
			return false;
		}
	);

	//速さ
	float kSpeed = 1.0f;

	worldTransform_.translation_.z += 0.2f;

	//Behaviorの遷移処理
	if (behaviorRequest_)
	{
		behavior_ = behaviorRequest_.value();
		//プレイヤーの各行動の初期化を実行
		switch (behavior_) {
		case Behavior::kNormal:
		default:
			BehaviorNormalInitialize();
			break;
		case Behavior::kShot:
			BehaviorShotInitialize();
			break;
		}
		behaviorRequest_ = std::nullopt;
	}

	//Behaviorの実行
	switch (behavior_)
	{
	case Behavior::kNormal:
	default:
		BehaviorNormalUpdate();
		break;
	case Behavior::kShot:
		BehaviorShotUpdate();
		break;
	}


	//ワールドトランスフォームの更新
	worldTransform_.quaternion_ = Mathseries::Normalize(Mathseries::Slerp(worldTransform_.quaternion_, destinationQuaternion_, 0.4f));
	worldTransform_.UpdateMatrixFromQuaternion();

	//モデルの更新
	model_->Update(worldTransform_, animationNumber_);

	//ReticleAim();

	//ミサイルの更新
	for (const std::unique_ptr<PlayerBullet>& bullets : bullet_)
	{
		bullets->Update();
	}

	//アニメーションを再生
	if (!model_->GetAnimation()->IsPlaying())
	{
		animationNumber_ = 0;
		model_->GetAnimation()->PlayAnimation();
	}

	ImGui::Begin("Player");
	ImGui::Text("AnimationNumber : %d", animationNumber_);
	ImGui::End();
}

void Player::BehaviorNormalInitialize()
{
	
}

void Player::BehaviorNormalUpdate()
{
	//移動処理
	const float speed = 0.6f;
	Move(speed);

	if (input_->IsControllerConnected())
	{
		//ダッシュ行動に変更
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_RIGHT_SHOULDER))
		{
				behaviorRequest_ = Behavior::kShot;
		}
	}

}

void Player::BehaviorShotInitialize()
{

}

void Player::BehaviorShotUpdate()
{
	//移動処理
	const float speed = 2.6f;
	Move(speed);

	ShotAttack();
	
	//通常行動に変更
	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_RIGHT_SHOULDER))
	{
		behaviorRequest_ = Behavior::kNormal;
	}
}

void Player::ShotAttack()
{
	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
	{
		PlayerBullet* newbullet = new PlayerBullet();
		Vector3 velocity[2];
		velocity[0] = { 0.0f,0.0f,0.01f };
		velocity[0] = Mathseries::TransformNormal(velocity[0], worldTransform_.matWorld_);
		Vector3 worldPos;
		worldPos = Player::GetWorldPosition();
		Vector3 translation = worldPos;
		newbullet->Initialize(translation, velocity[0]);
		//弾を登録する
		bullet_.push_back(std::unique_ptr<PlayerBullet>(newbullet));
	}
}

void Player::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, camera);
	//bulletの描画
	for (const std::unique_ptr<PlayerBullet>& bullets : bullet_)
	{
		bullets->Draw(camera);
	}
}

void Player::DrawUI()
{

}

void Player::OnCollision(Collider* collider)
{
	
}

void Player::SetParent(const WorldTransform* parent)
{

	worldTransform_.parent_ = parent;

}

void Player::SetRaticleParent(const WorldTransform* parent)
{

	worldTransform3DReticle_.parent_ = parent;

}
//
//const Vector3 Player::GetWorldPosition() const
//{
//	Vector3 pos{};
//	pos.x = worldTransform_.matWorld_.m[3][0];
//	pos.y = worldTransform_.matWorld_.m[3][1];
//	pos.z = worldTransform_.matWorld_.m[3][2];
//	return pos;
//}

Vector3 Player::Get3DReticleWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform3DReticle_.matWorld_.m[3][0];
	worldPos.y = worldTransform3DReticle_.matWorld_.m[3][1];
	worldPos.z = worldTransform3DReticle_.matWorld_.m[3][2];
	return worldPos;
}

void Player::MoveAnimation()
{
	
	
}

void Player::Move(const float speed)
{
	if (input_->IsControllerConnected())
	{
		//しきい値
		const float threshold = 0.7f;

		//移動フラグ
		bool isMoving = false;

		//移動量
		velocity_ = {
			input_->GetLeftStickX(),
			input_->GetLeftStickY(),
			0.0f,
		};

		//スティックの押し込みが遊び範囲を超えていたら移動フラグをtrueにする
		if (Mathseries::Length(velocity_) > threshold)
		{
			isMoving = true;
		}

		//スティックによる入力がある場合
		if (isMoving)
		{
			//移動量に速さを反映
			velocity_ = Mathseries::Normalize(velocity_) * speed;

			//移動
			worldTransform_.translation_ += velocity_;
		}
		else
		{
			velocity_ = { 0.0f,0.0f,0.0f };
		}
	}
}

void Player::ReticleAim()
{
	Set3DReticlePosition();
}

void Player::Set3DReticlePosition() {
	//// 自機から3Dレティクルへのオフセット(Z+向き)
	//Vector3 offset{ 0, 0, kDistanceObject };

	//// 回転行列を合成
	//Matrix4x4 rotateMatrix = Mathseries::MakeRotateMatrix(worldTransform3DReticle_.rotation_ + worldTransform3DReticle_.parent_->rotation_);
	//// 自機のワールド行列の回転を反映する
	//offset = Mathseries::TransformNormal(offset, rotateMatrix);

	//// 3Dレティクルの座標を設定
	//worldTransform3DReticle_.translation_ = GetWorldPosition() + offset;

	//worldTransform3DReticle_.quaternion_ = Mathseries::Normalize(Mathseries::Slerp(worldTransform3DReticle_.quaternion_, destinationQuaternion_, 0.4f));
	//worldTransform3DReticle_.UpdateMatrixFromQuaternion();

}



void Player::Rotate(const Vector3& v)
{

}