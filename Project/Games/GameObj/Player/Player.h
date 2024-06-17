#pragma once
#include "Engine/Framework/Object/IGameObject.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/2D/Sprite.h"
#include "Engine/Math/MathFunction.h"
#include <optional>

class LockOn;

class Player : public IGameObject, public Collider
{
public:
	//プレイヤーの状態
	enum class Behavior
	{
		kRoot,//通常状態
	};

	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camemra) override;

	void DrawUI() override;

	void OnCollision(Collider* collider) override;

	const Vector3 GetWorldPosition() const override;

	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

	void SetModels(const std::vector<Model*> models) { models_ = models; };

	void SetCamera(const Camera* camera) { camera_ = camera; };

	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }

	const Vector3& GetVelocity() const { return velocity_; };



private:
	void BehaviorRootInitialize();

	void BehaviorRootUpdate();

	void MoveAnimation();

	void Move(const float speed);

	void Rotate(const Vector3& v);

private:
	enum Parts
	{
		kBody,
		kHead,
		kL_Arm,
		kR_Arm,
		kCountOfParts
	};

	
private:
	//モデル
	std::vector<Model*> models_{};

	//ワールドトランスフォーム
	WorldTransform worldTransforms[kCountOfParts]{};

	//入力クラス
	Input* input_ = nullptr;

	//速度
	Vector3 velocity_{};

	
	//重力
	Vector3 gravity_{};

	//クォータニオン
	Quaternion destinationQuaternion_{ 0.0f,0.0f,0.0f,1.0f };

	bool isHit_ = false;

	//カメラ
	const Camera* camera_ = nullptr;

	//ロックオン
	const LockOn* lockOn_ = nullptr;

	//パーティクル
	std::unique_ptr<Model> particleModel_ = nullptr;
	ParticleSystem* particleSystem_ = nullptr;

	
	bool isDashAttack_ = false;

	//アニメーションの番号
	uint32_t animationNumber_ = 0;
};

