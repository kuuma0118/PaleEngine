#pragma once
#include "Engine/Framework/Object/IGameObject.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/2D/Sprite.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Math/WorldTransform.h"

/// <summary>
/// 自キャラの弾
/// </summary>
class PlayerBullet : public Collider {
public: // メンバ関数

	//弾が消える時間
	static const int32_t kBulletTime = 300;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="pos">初期座標</param>
	void Initialize(const Vector3& pos, const Vector3& velocity);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

    /// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>	
	void Draw(const Camera& camera);

	// Getter
	bool IsDead() const { return isDead_; }
	// ワールド行列の平行移動成分を取得
	const Vector3 GetWorldPosition() const override;
	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision(Collider* collider) override;

	/// <summary>
    /// ワールドポジションを取得
    /// </summary>
    /// <returns></returns>
	const Vector3 GetWorldPosition() const override;


private: // メンバ変数
	//モデル
	std::unique_ptr<Model> model_ = nullptr;

	//ワールドトランスフォーム
	WorldTransform worldTransform_{};

	//速度
	Vector3 velocity_{};

	//媒介変数
	float trackingParameter_ = 0.0f;

	//追尾
	bool isTrackingComplete_ = false;

	//追尾タイマー
	uint32_t trackingTimer_ = 0;

	//死亡フラグ
	bool isDead_ = false;
};

