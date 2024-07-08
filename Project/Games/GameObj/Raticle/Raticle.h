#pragma once
#include "Engine/Components/Input/Input.h"
#include "Engine/Base/WindowAppli.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/2D/Sprite.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Math/WorldTransform.h"
#include "Games/GameObj/Player/PlayerBullet.h"

class Raticle
{
public:

	Raticle();

	~Raticle();

	void Initialize();

	void Update(const Camera& camera);

	void Draw();

	Vector3 GetTargetPosition() const;

private:
	Input* input_ = nullptr;

	std::unique_ptr<Sprite> lockOnMark_ = nullptr;

    // 3Dレティクル用ワールドトランスフォーム
	WorldTransform worldTransform3DReticle_;

	// 2Dレティクル用のスプライト
	Sprite* sprite2DReticle_ = nullptr;
	
	//目標座標
	float destinationpositionX_ = 0.0f;
	float destinationpositionY_ = 0.0f;

	Matrix4x4 matViewport_;
	Vector2 spritePosition_;
};

