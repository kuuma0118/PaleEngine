#pragma once
#include "3D/Model/Model.h"
#include "3D/Matrix/WorldTransform.h"
#include "2D/ImGuiManager.h"
#include "Components/Input.h"
#include "Utility/CollisionManager/Collider.h"
#include "3D/Model/ParticleModel.h"
#include "3D/Particle/ParticleSystem.h"

class Player
{
public:

	~Player();

	void Initialize();

	void Update();

	void Draw(const ViewProjection viewProjection);

	void ApplyGlobalVariables();

	void DrawSprite();

	void DrawParticle(const ViewProjection& viewProjection);

private:
	Input* input_ = nullptr;

	// モデル
	std::unique_ptr<Model> model_;
	uint32_t playerTexture_ = 0;

	WorldTransform playerWorldTransform_;

	uint32_t textureHandle_ = 0u;

	int  motionMode_;
	Vector3 prePlayerTranslation_;

	WorldTransform platformMotionWorldTransform_;

	//そもそものサイズ
	Vector3 NormalScale_;

	//パーティクル
	std::unique_ptr<ParticleModel> particleModel_ = nullptr;
	std::unique_ptr<ParticleSystem> particleSystem_ = nullptr;
};



