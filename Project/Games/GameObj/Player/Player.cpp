#include "Player.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include <numbers>

void Player::Initialize()
{
	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_.z = 2.0f;
	
	//入力クラスのインスタンスを取得
	input_ = Input::GetInstance();

	//パーティクルシステムの初期化
	particleModel_.reset(ModelManager::CreateFromModelFile("Cube.obj", Opaque));
	particleSystem_ = ParticleManager::Create("Dash");
	particleSystem_->SetModel(particleModel_.get());
	particleSystem_->SetIsBillBoard(false);


	//衝突属性を設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionMaskPlayer);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
}

void Player::Update()
{

	//ヒットフラグのリセット
	isHit_ = false;

	//速さ
	float kSpeed = 1.0f;

	//移動量
	if (input_->IsControllerConnected())
	{
		velocity_ = {
			input_->GetLeftStickX(),
			0.0f,
			input_->GetLeftStickY(),
		};
	}

	worldTransform_.translation_ += velocity_;

	//移動量に速さを反映
	velocity_ = Mathf::Normalize(velocity_) * kSpeed;

	//移動限界座標
	const float kMoveLimitX = 49;
	const float kMoveLimitZ = 49;
	worldTransform_.translation_.x = std::max<float>(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = std::min<float>(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.z = std::max<float>(worldTransform_.translation_.z, -kMoveLimitZ);
	worldTransform_.translation_.z = std::min<float>(worldTransform_.translation_.z, +kMoveLimitZ);

	//ワールドトランスフォームの更新
	worldTransform_.quaternion_ = Mathf::Normalize(Mathf::Slerp(worldTransform_.quaternion_, destinationQuaternion_, 0.4f));
	worldTransform_.UpdateMatrixFromEuler();
	worldTransform_.UpdateMatrixFromQuaternion();

	//モデルの更新
	model_->Update(worldTransform_, animationNumber_);

	//アニメーションを再生
	if (!model_->GetAnimation()->IsPlaying())
	{
		animationNumber_ = 0;
		model_->GetAnimation()->PlayAnimation();
	}

	////移動処理
	//const float speed = 0.6f;
	//Move(speed);

	ImGui::Begin("Player");
	ImGui::Text("AnimationNumber : %d", animationNumber_);
	ImGui::End();
}

void Player::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, camera);
}

void Player::DrawUI()
{

}

void Player::OnCollision(Collider* collider)
{
	
}

const Vector3 Player::GetWorldPosition() const
{
	Vector3 pos{};
	pos.x = worldTransform_.matWorld_.m[3][0];
	pos.y = worldTransform_.matWorld_.m[3][1] + 1.0f;
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
			0.0f,
			input_->GetLeftStickY(),
		};

		//スティックの押し込みが遊び範囲を超えていたら移動フラグをtrueにする
		if (Mathf::Length(velocity_) > threshold)
		{
			isMoving = true;
		}

		//スティックによる入力がある場合
		if (isMoving)
		{
			//移動量に速さを反映
			velocity_ = Mathf::Normalize(velocity_) * speed;

			//移動ベクトルをカメラの角度だけ回転する
			Matrix4x4 rotateMatrix = Mathf::MakeRotateYMatrix(camera_->rotation_.y);
			velocity_ = Mathf::TransformNormal(velocity_, rotateMatrix);

			//移動
			worldTransform_.translation_ += velocity_;

			//回転
			Rotate(velocity_);
		}
		else
		{
			velocity_ = { 0.0f,0.0f,0.0f };
		}
	}
}

void Player::Rotate(const Vector3& v)
{
	Vector3 vector = Mathf::Normalize(v);
	Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, vector));
	float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, vector);
	destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
}