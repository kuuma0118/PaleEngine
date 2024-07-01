#include "GameTitleScene.h"
#include "Engine/Framework/Scene/SceneManager.h"	
#include "Engine/Base/TextureManager.h"	
#include <numbers>

void GameTitleScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//ゲームオブジェクトをクリア
	gameObjectManager_ = GameObjectManager::GetInstance();
	gameObjectManager_->Clear();

	camera_.Initialize();

	worldTransform_.Initialize();
	
	//プレイヤーの生成
	playerModel_.reset(ModelManager::CreateFromModelFile("walk.gltf", Opaque));
	playerModel_->GetMaterial()->SetEnableLighting(true);
	playerModel_->GetMaterial()->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	player_ = GameObjectManager::CreateGameObject<Player>();
	player_->SetModel(playerModel_.get());
	player_->SetTag("Player");

}

void GameTitleScene::Finalize()
{

}

void GameTitleScene::Update() 
{

	camera_.UpdateMatrix();

	//ゲームオブジェクトの更新
	gameObjectManager_->Update();

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