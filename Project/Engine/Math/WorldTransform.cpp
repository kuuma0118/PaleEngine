#include "WorldTransform.h"
#include "Engine/Math/MathFunction.h"

void WorldTransform::Initialize()
{
	constBuff_ = std::make_unique<UploadBuffer>();
	constBuff_->Create(sizeof(ConstBuffDataWorldTransform));
	UpdateMatrixFromEuler();
}

void WorldTransform::TransferMatrix()
{
	ConstBuffDataWorldTransform* worldTransformData = static_cast<ConstBuffDataWorldTransform*>(constBuff_->Map());
	worldTransformData->world = matWorld_;
	worldTransformData->worldInverseTranspse = Mathseries::Transpose(Mathseries::Inverse(matWorld_));
	constBuff_->Unmap();
}

void WorldTransform::UpdateMatrixFromEuler()
{
	matWorld_ = Mathseries::MakeAffineMatrix(scale_, rotation_, translation_);

	if (parent_) 
	{
		matWorld_ = matWorld_ * parent_->matWorld_;
	}

	TransferMatrix();
}

void WorldTransform::UpdateMatrixFromQuaternion()
{
	matWorld_ = Mathseries::MakeAffineMatrix(scale_, quaternion_, translation_);

	if (parent_)
	{
		matWorld_ = matWorld_ * parent_->matWorld_;
	}

	TransferMatrix();
}