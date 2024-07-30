#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/2D/Sprite.h"	
#include "Engine/Math/MathFunction.h"
#include "Games/GameObj/Player/Player.h"
#include "Games/GameObj/NamedEnemy/NamedEnemy.h"
#include "Games/GameObj/Enemy/Enemy.h"
#include "Games/GameObj/Player/SkyDome.h"
#include "Games/GameObj/RailCamera/RailCamera.h"
#include <Engine/Components/Collision/CollisionManager.h>

class GameTitleScene : public IScene
{
public:

	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameTitleScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameTitleScene();

	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void DrawUI() override;

	void UpdateTransition();

	/// <summary>
	/// 敵弾を追加する
	/// </summary>
	/// <param name="enemyBullet"></param>
	void AddEnemyBullet(EnemyBullet* enemyBullet);

	/// <summary>
	/// 敵発生データの読み込み
	/// </summary>
	void LoadEnemyPopData();

	/// <summary>
	/// 敵発生コマンドの更新
	/// </summary>
	void UpdateEnemyPopCommands();

	/// <summary>
	/// 敵生成
	/// </summary>
	void AddEnemy(const Vector3& pos);

private:
	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	Camera camera_{};

	WorldTransform worldTransform_{};

	// 衝突マネージャー
	CollisionManager* collisionManager_ = nullptr;

	//ゲームオブジェクトマネージャー	
	GameObjectManager* gameObjectManager_ = nullptr;

	//プレイヤー
	std::unique_ptr<Model> playerModel_ = nullptr;
	WorldTransform playerWorldTransform_{};
	Player* player_ = nullptr;

	// 敵
	//std::list<Enemy*> enemys_;
	//std::unique_ptr<Model> enemyModel_ = nullptr;
	//Enemy* enemy_ = nullptr;
	//WorldTransform enemyWorldTransform_{};

	std::unique_ptr<Model> namedEnemyModel_ = nullptr;
	WorldTransform namedEnemyWorldTransform_ = {};

	//待機中フラグ
	bool waitingFlag;

	uint32_t waitTimer;

	//敵発生コマンド
	std::stringstream enemyPopCommands;

	// 弾
	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;

	//天球
	std::unique_ptr<Model> skydomeModel_ = nullptr;
	Skydome* skydome_ = nullptr;

	//RailCamera
	RailCamera* railCamera_ = nullptr;

	std::unique_ptr<Model> model_ = nullptr;
};

