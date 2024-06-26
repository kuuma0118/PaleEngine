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
			input_->GetLeftStickY(),
			0.0f,
		};
	}

	worldTransform_.translation_ += velocity_;

	//速さを加算
	velocity_ = Mathf::Normalize(velocity_) * kSpeed;

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
	
}

void Player::Rotate(const Vector3& v)
{

}