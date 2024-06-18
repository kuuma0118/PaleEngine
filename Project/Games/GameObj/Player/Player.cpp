#include "Player.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include <numbers>

void Player::Initialize()
{
	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_.z = -20.0f;
	//worldTransform_.scale_ = { 0.8f,0.8f,0.8f };

	//パーツのワールドトランスフォームの初期化
	for (uint32_t i = 0; i < kCountOfParts; ++i)
	{
		worldTransforms[i].Initialize();
	}

	
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



	//移動限界座標
	const float kMoveLimitX = 49;
	const float kMoveLimitZ = 49;
	worldTransform_.translation_.x = std::max<float>(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = std::min<float>(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.z = std::max<float>(worldTransform_.translation_.z, -kMoveLimitZ);
	worldTransform_.translation_.z = std::min<float>(worldTransform_.translation_.z, +kMoveLimitZ);

	//ワールドトランスフォームの更新
	worldTransform_.quaternion_ = Mathf::Normalize(Mathf::Slerp(worldTransform_.quaternion_, destinationQuaternion_, 0.4f));
	worldTransform_.UpdateMatrixFromQuaternion();
	for (uint32_t i = 0; i < kCountOfParts; ++i)
	{
		worldTransforms[i].UpdateMatrixFromEuler();
	}

	//モデルの更新
	models_[0]->Update(worldTransform_, animationNumber_);

	MoveAnimation();

	models_[0]->GetAnimation()->PlayAnimation();

	ImGui::Begin("Player");
	ImGui::Text("AnimationNumber : %d", animationNumber_);
	ImGui::End();
}

void Player::Draw(const Camera& camera)
{
	//プレイヤーのモデルの描画
	//for (uint32_t i = 0; i < kCountOfParts; ++i)
	//{
	//	models_[i]->Draw(worldTransforms[i], camera);
	//}
	models_[0]->Draw(worldTransform_, camera);

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


void Player::BehaviorRootInitialize()
{
	gravity_ = { 0.0f,0.0f,0.0f };
	if (!particleSystem_->GetParticleEmitter("Move"))
	{
		Vector3 translation = { worldTransforms[kBody].matWorld_.m[3][0], 0.0f ,worldTransforms[kBody].matWorld_.m[3][2] };
		ParticleEmitter* emitter = ParticleEmitterBuilder()
			.SetDeleteTime(60)
			.SetEmitterName("Move")
			.SetPopArea({ -0.5f,0.0f,-0.5f }, { 0.5f,0.0f,0.5f })
			.SetPopAzimuth(0.0f, 0.0f)
			.SetPopColor({ 0.1f,0.1f,0.1f,1.0f }, { 0.1f,0.1f,0.1f,1.0f })
			.SetPopCount(1)
			.SetPopElevation(0.0f, 0.0f)
			.SetPopFrequency(0.01f)
			.SetPopLifeTime(0.2f, 0.4f)
			.SetPopQuaternion(destinationQuaternion_)
			.SetPopRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetPopScale({ 0.1f,0.1f,0.1f }, { 0.2f,0.2f,0.2f })
			.SetPopVelocity({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetTranslation(translation)
			.Build();
		particleSystem_->AddParticleEmitter(emitter);
	}
	worldTransforms[kBody].rotation_.x = 0.0f;
	worldTransforms[kBody].rotation_.y = 0.0f;
}

void Player::BehaviorRootUpdate()
{
	//移動処理
	const float speed = 0.6f;
	Move(speed);

	//移動しているときにアニメーションをする
	if (velocity_ != Vector3{ 0.0f,0.0f,0.0f })
	{
		MoveAnimation();

		//アニメーションを再生
		if (!models_[0]->GetAnimation()->IsPlaying())
		{
			animationNumber_ = 0;
			models_[0]->GetAnimation()->PlayAnimation();
		}

		//パーティクルの座標を更新
		Vector3 translation = { worldTransforms[kBody].matWorld_.m[3][0], 0.0f ,worldTransforms[kBody].matWorld_.m[3][2] };
		ParticleEmitter* emitter = particleSystem_->GetParticleEmitter("Move");
		if (emitter)
		{
			emitter->SetPopCount(1);
			emitter->SetTranslation(translation);
		}
		//エミッターが消えてたら再生成
		else
		{
			ParticleEmitter* emitter = ParticleEmitterBuilder()
				.SetDeleteTime(60)
				.SetEmitterName("Move")
				.SetPopArea({ -0.5f,0.0f,-0.5f }, { 0.5f,0.0f,0.5f })
				.SetPopAzimuth(0.0f, 0.0f)
				.SetPopColor({ 0.1f,0.1f,0.1f,1.0f }, { 0.1f,0.1f,0.1f,1.0f })
				.SetPopCount(1)
				.SetPopElevation(0.0f, 0.0f)
				.SetPopFrequency(0.01f)
				.SetPopLifeTime(0.2f, 0.4f)
				.SetPopQuaternion(destinationQuaternion_)
				.SetPopRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetPopScale({ 0.1f,0.1f,0.1f }, { 0.2f,0.2f,0.2f })
				.SetPopVelocity({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetTranslation(translation)
				.Build();
			particleSystem_->AddParticleEmitter(emitter);
		}
	}
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