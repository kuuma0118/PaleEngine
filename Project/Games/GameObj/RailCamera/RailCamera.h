#pragma once
#include"Engine/Math/WorldTransform.h"
#include"Engine/3D/Camera/Camera.h"
#include"Engine/Math/MathFunction.h"

class RailCamera {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Vector3 translation, Vector3 rotation);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// ビュープロジェクションを取得
	/// </summary>
	/// <returns>ビュープロジェクション</returns>
	const Camera& GetViewProjection() { return camera_; }

	/// <summary>
	/// ワールドトランスフォームを取得
	/// </summary>
	/// <returns></returns>
	const WorldTransform& GetWorldTransform() { return worldTransform_; };

private:
	WorldTransform worldTransform_;
	Camera camera_;
};