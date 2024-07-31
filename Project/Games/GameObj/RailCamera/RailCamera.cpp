#include "RailCamera.h"

RailCamera* RailCamera::GetInstance()
{
	static RailCamera instance;
	return &instance;
}

void RailCamera::Initialize(Vector3 translation, Vector3 rotation) {
	//ワールドトランスフォームの初期設定
	worldTransform_.translation_ = translation;
	worldTransform_.rotation_ = rotation;
	//ビュープロジェクションの初期化
	camera_.Initialize();
};

void RailCamera::Update() {
	Vector3 move{ 0, 0, 0 };
	const float kCameraSpeed = 0.2f;
	const float kRotSpeed = -0.0f;

	// 行列の更新
	move.z -= kCameraSpeed;
	Move(worldTransform_.translation_, move);
	worldTransform_.rotation_.y += kRotSpeed;
	worldTransform_.matWorld_ = Mathseries::MakeAffineMatrix(
	worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);


	//行列の更新
	camera_.UpdateMatrix();

};