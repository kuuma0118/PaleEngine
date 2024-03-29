#pragma once
#include "IScene.h"
#include "Base/TextureManager.h"
#include "Components/Audio.h"
#include "Components/Input.h"
#include "2D/ImGuiManager.h"
#include "3D/DebugCamera/DebugCamera.h"

#include "3D/Model/Model.h"
#include "3D/Model/ParticleModel.h"
#include "2D/Sprite.h"
#include "3D/Matrix/WorldTransform.h"
#include "3D/Matrix/ViewProjection.h"
#include "Utility/CollisionManager/CollisionManager.h"

#include <memory>
#include "../GameObject/Player/Player.h"

class GameScene : public IScene
{
public:
	//トランジションの時間
	static const int kTransitionTime = 60;
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
	void Initialize(GameManager* gameManager)override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update(GameManager* gameManager)override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(GameManager* gameManager)override;

private:
	//TextureManager
	TextureManager* textureManager_ = nullptr;
	//Audio
	Audio* audio_ = nullptr;
	//Input
	Input* input_ = nullptr;
	//DebugCamera
	DebugCamera* debugCamera_ = nullptr;
	//DebugCameraの切り替え
	bool isDebugCameraActive_ = false;
	//カメラ
	ViewProjection viewProjection_{};

	//テクスチャ
	uint32_t textureHandle_ = 0;

	//サウンド
	uint32_t soundHandle_ = 0u;

	//ワールドトランスフォーム
	WorldTransform worldTransform1_{};
	WorldTransform worldTransform2_{};


	XINPUT_STATE joyState_;
	//プレスAのスプライト
	std::unique_ptr<Sprite>  pressASprite_ = nullptr;
	//プレスAのテクスチャ
	uint32_t pressATextureHandle_ = 0;
	//トランジション用のスプライト
	std::unique_ptr<Sprite> transitionSprite_ = nullptr;
	//トランジションのテクスチャ
	uint32_t transitionTextureHandle_ = 0;
	//トランジションの色
	Vector4 transitionColor_ = { 0.0f,0.0f,0.0f,1.0f };
	//トランジションのフラグ
	bool isTransition_ = false;
	bool isTransitionEnd_ = false;
	//トランジションのタイマー
	float transitionTimer_ = 0;
	std::unique_ptr<Player> player_ = nullptr;

};