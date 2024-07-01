#include "Block.h"

void Block::Initialize()
{
	worldTransform_.Initialize();
	worldTransform_.translation_.y = -7.0f;
	worldTransform_.translation_.x = 7.0f;
	worldTransform_.scale_ = { 3.0f,3.0f,3.0f };
}

void Block::Update()
{
	worldTransform_.UpdateMatrixFromEuler();
}

void Block::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, camera);
}

void Block::DrawUI()
{

}