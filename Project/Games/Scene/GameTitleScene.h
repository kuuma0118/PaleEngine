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
#include "Games/GameObj/Player/PlayerTypeB.h"
#include "Games/GameObj/Player/Block.h"

class GameTitleScene : public IScene
{
public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void DrawUI() override;

	void UpdateTransition();

private:
	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	Camera camera_{};

	WorldTransform worldTransform_{};

	//ゲームオブジェクトマネージャー	
	GameObjectManager* gameObjectManager_ = nullptr;

	//プレイヤー
	std::unique_ptr<Model> playerModel_ = nullptr;
	WorldTransform playerWorldTransform_{};
	Player* player_ = nullptr;

	//プレイヤーtypeB
	std::unique_ptr<Model> playerModelTypeB_ = nullptr;
	WorldTransform playerWorldTransformTypeB_{};
	PlayerTypeB* playerTypeB_ = nullptr;

	//ブロック
	std::unique_ptr<Model> blockModel_ = nullptr;
	Block* block_ = nullptr;

	std::unique_ptr<Model> model_ = nullptr;
};

