#include "Player.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include <numbers>

void Player::Initialize()
{
	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_.z = 2.0f;
	worldTransform_.scale_ = {5.0f,5.0f,5.0f};
	
	//入力クラスのインスタンスを取得
	input_ = Input::GetInstance();

	
}

void Player::Update()
{

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



	//bulletの更新
	for (const std::unique_ptr<PlayerBullet>& missile : bullet_)
	{
		missile->Update();
	}

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
	const float speed = 0.6f;
	Move(speed);

	const float kBulletSpeed = 1.0f;
	Vector3 velocity(0, 0, kBulletSpeed);

	velocity = Mathseries::TransformNormal(velocity, worldTransform_.matWorld_);

	ShotAttack();

	//通常行動に変更
	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_RIGHT_SHOULDER))
	{
		behaviorRequest_ = Behavior::kNormal;
	}
}

void Player::ShotAttack()
{
	// 弾の速度
	const float kBulletSpeed = 1.0f;
	Vector3 velocity(0, 0, kBulletSpeed);

	// 速度ベクトルを自機の向きに合わせて回転させる
	velocity = Mathseries::TransformNormal(velocity, worldTransform_.matWorld_);

	//// 弾を生成し、初期化
	//PlayerBullet* newBullet = new PlayerBullet();
	//Vector3 worldPos{};
	//worldPos = Player::GetWorldPosition();
	//newBullet->Initialize(worldPos, velocity);
}

void Player::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, camera);
	//bulletの描画
	for (const std::unique_ptr<PlayerBullet>& bullet : bullet_)
	{
		bullet->Draw(camera);
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

const Vector3 Player::GetWorldPosition() const
{
	Vector3 pos{};
	pos.x = worldTransform_.matWorld_.m[3][0];
	pos.y = worldTransform_.matWorld_.m[3][1];
	pos.z = worldTransform_.matWorld_.m[3][2];
	return pos;
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

void Player::Rotate(const Vector3& v)
{

}