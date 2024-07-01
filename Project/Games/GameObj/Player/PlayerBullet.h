#pragma once
#include "Engine/Framework/Object/IGameObject.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/2D/Sprite.h"
#include "Engine/Math/MathFunction.h"

/// <summary>
/// 自キャラの弾
/// </summary>
class PlayerBullet : public Collider {
public: // メンバ関数
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

private: // メンバ変数
	// ワールド変換データ
	//WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t bulletTexture_ = 0u;

	// 寿命<frm>
	static const int32_t kLifeTime = 60 * 5;
	// 死亡タイマー
	int32_t deathTimer_ = kLifeTime;
	// 死亡フラグ
	bool isDead_ = false;

	// 速度
	Vector3 velocity_;
};

