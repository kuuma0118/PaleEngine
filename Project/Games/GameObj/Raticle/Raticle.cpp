#include "Raticle.h"

Raticle::Raticle() {}

Raticle::~Raticle() {
	delete sprite2DReticle_;
}

void Raticle::Initialize()
{
	//インスタンスを取得
	input_ = Input::GetInstance();

	//スプライトの生成
	TextureManager::Load("Reticle.png");
	lockOnMark_.reset(Sprite::Create("Reticle.png", { 0.0f,0.0f }));
	lockOnMark_->SetAnchorPoint({ 0.5f,0.5f });

	// 3Dレティクルのワールドトランスフォーム初期化
	worldTransform3DReticle_.Initialize();

	// スプライトの現在座標を取得
	spritePosition_ = sprite2DReticle_->GetPosition();
}

void Raticle::Update(const Camera& camera)
{
	//旋回操作
	if (input_->IsControllerConnected())
	{

		//しきい値
		const float threshold = 0.7f;

		//移動フラグ
		bool isRotation = false;

		//移動量
		Vector3 rotation = {
			input_->GetRightStickY(),
			0.0f,
			input_->GetRightStickX()
		};

		//スティックの押し込みが遊び範囲を超えていたら移動フラグをtureにする
		if (Mathf::Length(rotation) > threshold)
		{
			isRotation = true;
		}

		if (isRotation)
		{
			//移動速度
			const float kRotSpeedX = 0.02f;
			const float kRotSpeedY = 0.06f;

			spritePosition_.x -= rotation.x * kRotSpeedX;
			spritePosition_.y += rotation.y * kRotSpeedY;
		}

	}

}

void Raticle::Draw()
{
	lockOnMark_->Draw();
}

Vector3 Raticle::GetTargetPosition() const
{
	
	
	return Vector3();
}