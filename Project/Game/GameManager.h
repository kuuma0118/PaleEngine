#include "IScene.h"
#include "Base/D3DResourceLeakChecker.h"
#include "Base/WinApp.h"
#include "Base/FireControlSystem.h"
#include "Base/TextureManager.h"
#include "3D/Model/Model.h"
#include "2D/Sprite.h"
#include "2D/ImGuiManager.h"
#include "Components/Input.h"
#include "Components/Audio.h"
#include "Components/PostProcess.h"
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
	/// <param name="newScene"></param>
	void ChangeScene(IScene* newScene);

	/// <summary>
	/// ゲームループ
	/// </summary>
	void run();

private:
	//リークチェッカー
	D3DResourceLeakChecker leakCheck;
	//ウィンドウズアプリケーション
	WinApp* winApp_ = nullptr;
	//DirectX基盤クラス
	FCS* dxCommon_ = nullptr;
	//テクスチャ管理クラス
	TextureManager* textureManager_ = nullptr;
	//ImGuiManager
	ImGuiManager* imguiManager_ = nullptr;
	//入力クラス
	Input* input_ = nullptr;
	//オーディオクラス
	Audio* audio_ = nullptr;
	//ポストプロセス
	PostProcess* postProcess_ = nullptr;
	//現在のシーン
	IScene* currentScene_ = nullptr;

};