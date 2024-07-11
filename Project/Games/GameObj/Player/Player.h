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

#include "PlayerBullet.h"

class LockOn;

class Player : public IGameObject, public Collider
{
public:
	//プレイヤーの状態
	enum class Behavior
	{
		kNormal,//通常状態
		kShot, //通常攻撃状態
	};

	/// <summary>
    /// 初期化
    /// </summary>
    /// <param name="model">モデル</param>
    /// <param name="pos">初期座標</param>
	void Initialize() override;

	/// <summary>
    /// 更新
    /// </summary>
	void Update() override;

	/// <summary>
    /// 描画
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
	void Draw(const Camera& camemra) override;

	void DrawUI() override;

	void OnCollision(Collider* collider) override;

	const Vector3 GetWorldPosition() const override;

	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

	void SetCamera(const Camera* camera) { camera_ = camera; };

	const Vector3& GetVelocity() const { return velocity_; };

	void ShotAttack();

private:

	void BehaviorNormalInitialize();

	void BehaviorNormalUpdate();

	void BehaviorShotInitialize();

	void BehaviorShotUpdate();

	void MoveAnimation();

	void Move(const float speed);

	void Rotate(const Vector3& v);
	
private:

	//ワールドトランスフォーム
	WorldTransform worldTransforms_{};

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

	//プレイヤー弾
	//std::list<std::unique_ptr<PlayerBullet>> bullet_{};

	//プレイヤーの行動状態
	Behavior behavior_ = Behavior::kNormal;

	//次にする行動
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	//パーティクル
	std::unique_ptr<Model> particleModel_ = nullptr;
	ParticleSystem* particleSystem_ = nullptr;

	//アニメーションの番号
	uint32_t animationNumber_ = 0;
};

