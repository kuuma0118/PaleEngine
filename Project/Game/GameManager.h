#include "Base/WinApp.h"
#include "Base/DirectXCommon.h"
#include "Base/TextureManager.h"
#include "Base/D3DResourceLeakChecker.h"
#include "Components/Audio.h"
#include "Components/Input.h"
#include "Components/PostProcess.h"
#include "3D/Model/Model.h"
#include "3D/Model/ParticleModel.h"
#include "2D/Sprite.h"
#include "2D/ImGuiManager.h"
#include "Utility/Random.h"
#include "IScene.h"
#include <memory>

class GameManager {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameManager();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameManager();

	/// <summary>
	/// シーン切り替え
	/// </summary>
	/// <param name="newScene">新しいシーン</param>
	void ChangeScene(IScene* newScene);

	/// <summary>
	/// ゲームループ
	/// </summary>
	void run();

private:
	
	WinApp* winApp_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;
	TextureManager* textureManager_ = nullptr;
	ImGuiManager* imguiManager_ = nullptr;
	Audio* audio_ = nullptr;
	Input* input_ = nullptr;
	PostProcess* postProcess_ = nullptr;
	IScene* currentScene_;
};