#pragma once
#include "Mesh.h"
#include "Material.h"
#include "DirectionalLight.h"
#include "Base/DirectXCommon.h"
#include "Base/TextureManager.h"
#include "3D/Matrix/WorldTransform.h"
#include "3D/Matrix/ViewProjection.h"
#include "Utility/MathFunction.h"
#include "Utility/pch.h"

#include<vector>
#include<format>
#include<xstring>
#include <cassert>
#include <dxcapi.h>
#include <fstream>
#include <list>
#include <string>
#include <sstream>
#pragma comment(lib,"dxcompiler.lib")

/ class Model
{
public:
	enum DiffuseReflectionType
	{
		LambertianReflectance,
		HalfLambert,
	};

	enum SpecularReflectionType
	{
		PhongReflectionModel,
		BlinnPhongReflectionModel,
		NoSpecularReflection,
	};

	//ノード構造体
	struct Node {
		Matrix4x4 localMatrix{};
		std::string name;
		std::vector<Node> children;
	};

	//マテリアルデータ構造体
	struct MaterialData {
		std::string textureFilePath;
	};

	//モデルデータ構造体
	struct ModelData {
		std::vector<VertexDataPosUVNormal> vertices;
		MaterialData material;
		Node rootNode;
	};

	//Keyframe構造体
	template <typename tValue>
	struct Keyframe {
		float time;
		tValue value;
	};
	using KeyframeVector3 = Keyframe<Vector3>;
	using KeyframeQuaternion = Keyframe<Quaternion>;

	//AnimationCurve構造体
	template <typename tValue>
	struct AnimationCurve {
		std::vector<Keyframe<tValue>> keyframes;
	};

	//NodeAnimation構造体
	struct NodeAnimation {
		AnimationCurve<Vector3> translate;
		AnimationCurve<Quaternion> rotate;
		AnimationCurve<Vector3> scale;
	};

	//Animation構造体
	struct Animation {
		float duration;//アニメーション全体の尺(単位は秒)
		//NodeAnimationの集合。Node名でひけるようにしておく
		std::map<std::string, NodeAnimation> nodeAnimations;
	};

	void Create(const ModelData& modelData, const Animation& animationData, DrawPass drawPass);

	void Update(WorldTransform& worldTransform);

	void Draw(WorldTransform& worldTransform, const Camera& camera);

	const Vector4& GetColor() const { return color_; };

	void SetColor(const Vector4& color) { color_ = color; };

	const Vector2& GetUVScale() const { return uvScale_; };

	void SetUVScale(const Vector2& uvScale) { uvScale_ = uvScale; };

	const float GetUVRotation() const { return uvRotation_; };

	void SetUVRotation(const float uvRotation) { uvRotation_ = uvRotation; };

	const Vector2& GetUVTranslation() const { return uvTranslation_; };

	void SetUVTranslation(const Vector2& uvTranslation) { uvTranslation_ = uvTranslation; };

	const int32_t& GetEnableLighting() const { return enableLighting_; };

	void SetEnableLighting(const int32_t enableLighting) { enableLighting_ = enableLighting; };

	const int32_t& GetDiffuseReflectionType() const { return int32_t(diffuseReflectionType_); };

	void SetDiffuseReflectionType(const int32_t diffuseReflectionType) { diffuseReflectionType_ = DiffuseReflectionType(diffuseReflectionType); };

	const int32_t& GetSpecularReflectionType() const { return int32_t(specularReflectionType_); };

	void SetSpecularReflectionType(const int32_t specularReflectionType) { specularReflectionType_ = SpecularReflectionType(specularReflectionType); };

	const float& GetShininess() const { return shininess_; };

	void SetShininess(const float shininess) { shininess_ = shininess; };

	const Vector3& GetSpecularColor() const { return specularColor_; };

	void SetSpecularColor(const Vector3& specularColor) { specularColor_ = specularColor; };

	void SetTexture(const std::string& textureName);

private:
	void CreateVertexBuffer();

	void CreateMaterialConstBuffer();

	void UpdateMaterailConstBuffer();

	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);

	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

private:
	ModelData modelData_{};

	Animation animationData_{};

	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	std::unique_ptr<UploadBuffer> materialConstBuffer_ = nullptr;

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	Vector2 uvScale_ = { 1.0f,1.0f };

	float uvRotation_ = 0.0f;

	Vector2 uvTranslation_ = { 0.0f,0.0f };

	int32_t enableLighting_ = true;

	DiffuseReflectionType diffuseReflectionType_ = DiffuseReflectionType::HalfLambert;

	SpecularReflectionType specularReflectionType_ = SpecularReflectionType::BlinnPhongReflectionModel;

	float shininess_ = 40.0f;

	Vector3 specularColor_ = { 1.0f,1.0f,1.0f };

	DrawPass drawPass_ = Opaque;

	const Texture* texture_ = nullptr;

	float animationTime_ = 0.0f;

	friend class ParticleSystem;
};