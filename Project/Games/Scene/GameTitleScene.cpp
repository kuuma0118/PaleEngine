#include "GameTitleScene.h"
#include "Engine/Framework/Scene/SceneManager.h"	
#include "Engine/Base/TextureManager.h"	
#include <numbers>

GameTitleScene::GameTitleScene() {}

GameTitleScene::~GameTitleScene() {
	delete railCamera_;

}

void GameTitleScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	// RailCamera
	railCamera_ = new RailCamera();

	Vector3 radian = { 0.0f, 0.0f, 0.0f };

	//ゲームオブジェクトをクリア
	gameObjectManager_ = GameObjectManager::GetInstance();
	gameObjectManager_->Clear();

	camera_.Initialize();

	worldTransform_.Initialize();
	
	//プレイヤーの生成
	player_ = gameObjectManager_->GetGameObject<Player>("Player");
	player_->SetCamera(&camera_);
	player_->GetCollider()->SetCollisionAttribute(kCollisionAttributePlayer);
	player_->GetCollider()->SetCollisionMask(kCollisionMaskPlayer);
	player_->GetCollider()->SetCollisionPrimitive(kCollisionPrimitiveAABB);

	railCamera_->Initialize(player_->GetWorldPosition(), radian);
	player_->SetParent(&railCamera_->GetWorldTransform());

	namedEnemyModel_.reset(ModelManager::CreateFromModelFile("sneakWalk.gltf", Opaque));
	namedEnemyModel_->GetMaterial()->SetEnableLighting(false);
	namedEnemyModel_->GetMaterial()->SetColor({ 0.9f, 0.5f, 0.9f, 1.0f });
	namedEnemyWorldTransform_.Initialize();
	namedEnemyWorldTransform_.translation_.y = 3.0f;
	namedEnemyWorldTransform_.scale_ = { 3.0f,3.0f,3.0f };
	namedEnemyWorldTransform_.quaternion_ = Mathseries::MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, std::numbers::pi_v<float>);

	////enemyの生成
	//enemyModel_.reset(ModelManager::CreateFromModelFile("Cube.obj", Opaque));
	//enemyModel_->GetMaterial()->SetEnableLighting(false);
	//enemy_ = GameObjectManager::CreateGameObject<Enemy>();
	//enemy_->SetModel(enemyModel_.get());

	//敵発生データ読み込み
	//LoadEnemyPopData();
	//// 衝突マネージャーの生成
	//collisionManager_ = new CollisionManager();

	//天球の作成
	skydomeModel_.reset(ModelManager::CreateFromModelFile("Skydome.obj", Opaque));
	skydomeModel_->GetMaterial()->SetEnableLighting(false);
	skydome_ = gameObjectManager_->GetGameObject<Skydome>("Skydome");
	skydome_->SetModel(skydomeModel_.get());

}

void GameTitleScene::Finalize()
{

}

void GameTitleScene::Update() 
{

	// Camera
	railCamera_->Update();

	//ゲームオブジェクトの更新
	gameObjectManager_->Update();

	camera_.matView_ = railCamera_->GetViewProjection().matView_;
	camera_.matProjection_ = railCamera_->GetViewProjection().matProjection_;
	camera_.TransferMatrix();

	namedEnemyWorldTransform_.UpdateMatrixFromQuaternion();

	namedEnemyModel_->Update(namedEnemyWorldTransform_, 0);

	ImGui::Begin("GameTitleScene");
	ImGui::DragFloat3("WorldTransform.translation", &playerWorldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("WorldTransform.rotation", &playerWorldTransform_.rotation_.x, 0.1f);
	ImGui::DragFloat3("WorldTransform.scale", &playerWorldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("Camera.translation", &camera_.translation_.x, 0.1f);
	ImGui::DragFloat3("Camera.rotation", &camera_.rotation_.x, 0.1f);
	ImGui::End();
}

void GameTitleScene::Draw()
{
#pragma region 背景スプライト描画
	//背景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//背景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion

	//深度バッファをクリア
	renderer_->ClearDepthBuffer();

#pragma region 3Dオブジェクト描画

	////enemyのモデルの描画
	//enemyModel_->Draw(enemyWorldTransform_, camera_);

	namedEnemyModel_->Draw(namedEnemyWorldTransform_, camera_);

	//ゲームオブジェクトのモデル描画
	gameObjectManager_->Draw(camera_);
	//3Dオブジェクト描画
	renderer_->Render();
#pragma endregion

#pragma region パーティクル描画
	//パーティクル描画前処理
	renderer_->PreDrawParticles();

	//パーティクル描画後処理
	renderer_->PostDrawParticles();
#pragma endregion
}

void GameTitleScene::DrawUI() 
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

//void GameTitleScene::AddEnemy(const Vector3& pos) {
//	// 敵の初期化
//	enemyWorldTransform_.Initialize();
//	enemyWorldTransform_.translation_ = pos;
//	enemyWorldTransform_.scale_ = { 3.0f,3.0f,3.0f };
//	enemyWorldTransform_.quaternion_ = Mathseries::MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, std::numbers::pi_v<float>);
//	enemy_->Initialize();
//	enemys_.push_back(enemy_);
//}
//
//void GameTitleScene::LoadEnemyPopData() {
//	//ファイルを開く
//	std::ifstream file;
//	file.open("Games/GameObj/csv/enemyPop.csv");
//	assert(file.is_open());
//
//	//ファイルの内容を文字列ストリームにコピー
//	enemyPopCommands << file.rdbuf();
//
//	//ファイルを閉じる
//	file.close();
//}
//
//void GameTitleScene::UpdateEnemyPopCommands() {
//
//	//enemyの生成
//	enemyModel_.reset(ModelManager::CreateFromModelFile("Cube.obj", Opaque));
//	enemyModel_->GetMaterial()->SetEnableLighting(false);
//	enemy_ = GameObjectManager::CreateGameObject<Enemy>();
//	enemy_->SetModel(enemyModel_.get());
//
//	//待機処理
//	if (waitingFlag) {
//		waitTimer--;
//		if (waitTimer <= 0) {
//			//待機完了
//			waitingFlag = false;
//		}
//		return;
//	}
//
//	//一桁分の文字列を入れる変数
//	std::string line;
//
//	//コマンド実行ループ
//	while (getline(enemyPopCommands, line)) {
//		//一桁分の文字列をストリームに変換して解析しやすくなる
//		std::istringstream line_stream(line);
//
//		std::string word;
//		//,区切りで行の先端文字列を取得
//		getline(line_stream, word, ',');
//
//		//"//"から始まる行はコメント
//		if (word.find("//") == 0) {
//			//コメント行を飛ばす
//			continue;
//		}
//
//		//POPコマンド
//		if (word.find("POP") == 0) {
//			// x座標
//			getline(line_stream, word, ',');
//			float x = (float)std::atof(word.c_str());
//
//			// y座標
//			getline(line_stream, word, ',');
//			float y = (float)std::atof(word.c_str());
//
//			// z座標
//			getline(line_stream, word, ',');
//			float z = (float)std::atof(word.c_str());
//
//			//敵を発生させる
//			AddEnemy(Vector3(x, y, z));
//		}
//
//		// WAITコマンド
//		else if (word.find("WAIT") == 0) {
//			getline(line_stream, word, ',');
//
//			// 待ち時間
//			int32_t waitTime = atoi(word.c_str());
//
//			//待機開始
//			waitingFlag = true;
//			waitTimer = waitTime;
//
//			//コマンドループを抜ける
//			break;
//		}
//	}
//}