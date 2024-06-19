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
	playerModel_->GetMaterial()->SetEnableLighting(false);
	playerModel_->GetMaterial()->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	playerWorldTransform_.Initialize();
	playerWorldTransform_.translation_.y = 1.0f;
	playerWorldTransform_.scale_ = { 1.0f,1.0f,1.0f };
	playerWorldTransform_.quaternion_ = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, std::numbers::pi_v<float>);


}

void GameTitleScene::Finalize()
{

}

void GameTitleScene::Update() 
{
	worldTransform_.UpdateMatrixFromEuler();

	//ゲームオブジェクトの更新
	gameObjectManager_->Update();

	playerModel_->Update(worldTransform_,0);

	camera_.UpdateMatrix();

	ImGui::Begin("GameTitleScene");
	ImGui::DragFloat3("WorldTransform.translation", &worldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("WorldTransform.rotation", &worldTransform_.rotation_.x, 0.1f);
	ImGui::DragFloat3("WorldTransform.scale", &worldTransform_.scale_.x, 0.1f);
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