#pragma once
#include "IScene.h"
#include "Components/Input.h"
#include "Components/Audio.h"
#include "Components/PostProcess.h"
#include "3D/Model/Model.h"
#include "2D/Sprite.h"
#include <Components/Particle/ParticleSystem.h>

class GameScene : public IScene {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(GameManager* gameManager) override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update(GameManager* gameManager) override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(GameManager* gameManager) override;

private:
	//モデル
	std::unique_ptr<Model> model_ = nullptr;
	//ワールドトランスフォーム
	WorldTransform worldTransform_{};
	//ポストプロセス
	PostProcess* postProcess_ = nullptr;
	//レーザーのスケール
	Vector3 laserScale_ = { 1.0f,10.0f,1.0f };
	//レーザーの速さ
	float laserSpeed_ = 2.0f;
	//パーティクルシステム
	ParticleSystem* particleSystem_ = nullptr;
};