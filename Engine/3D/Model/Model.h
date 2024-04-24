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

/// <summary>
	/// 頂点データ
	/// </summary>
struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct MaterialData
{
	uint32_t handle{};
	string textureFilePath{};
};

struct  SModelData
{
	vector<VertexData> vertices;
	MaterialData material;
	string texFilePath;
	uint32_t texHandle;
	string normalilePath;
	uint32_t normalTexHandle;
	string basefilePath;
	uint32_t baseTexHandle;
};

/// <summary>
/// 3Dモデル
/// </summary>
class Model {
private:
	//エイリアステンプレート
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	/// <summary>
	/// ルートパラメータの番号
	/// </summary>
	enum class RootParameterIndex {
		Material,
		WorldlTransform,
		ViewProjection,
		Texture,
		DirectionalLight
	};

	/// <summary>
	/// マテリアルデータ構造体
	/// </summary>
	struct MaterialData {
		std::string textureFilePath;
	};

	/// <summary>
	/// モデルデータ構造体
	/// </summary>
	struct ModelData {
		std::vector<Mesh::VertexData> vertices;
		MaterialData material;
		std::string name;
	};

	/// <summary>
	/// 定数バッファ用の構造体
	/// </summary>
	struct ConstBufferDataTransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	static Model* GetInstance();

	/// <summary>
	/// 静的初期化
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// 静的メンバ変数の解放
	/// </summary>
	static void Release();

	/// <summary>
	/// モデルの作成
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="filename"></param>
	/// <returns>モデル</returns>
	static Model* CreateFromOBJ(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// 球の作成
	/// </summary>
	/// <returns>球</returns>
	static Model* CreateSphere();

	/// <summary>
	/// 描画前処理
	/// </summary>
	static void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="worldTransform">ワールドトランスフォーム</param>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection);

	/// <summary>
	/// 描画(テクスチャ指定)
	/// </summary>
	/// <param name="worldTransform">ワールドトランスフォーム</param>
	/// <param name="viewProjection">ビュープロジェクション</param>
	/// <param name="textureHandle">テクスチャハンドル</param>
	void Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, uint32_t textureHandle);

	/// <summary>
	/// DirectionalLightを取得
	/// </summary>
	/// <returns>DirectionalLight</returns>
	DirectionalLight* GetDirectionalLight() { return directionalLight_.get(); };

	/// <summary>
	/// マテリアルを取得
	/// </summary>
	/// <returns>マテリアル</returns>
	Material* GetMaterial() { return material_.get(); };

private:
	/// <summary>
	/// DXCの初期化
	/// </summary>
	static void InitializeDXC();

	/// <summary>
	/// シェーダーをコンパイルする
	/// </summary>
	/// <param name="filePath">Compilerするshaderのファイルパス</param>
	/// <param name="profile">Compilerで使用するProfile</param>
	/// <returns>実行用のバイナリ</returns>
	static ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile);

	/// <summary>
	/// PSOの作成
	/// </summary>
	static void CreatePipelineStateObject();

	/// <summary>
	/// Objファイルの読み込み
	/// </summary>
	/// <param name="directoryPath">ディレクトリ名</param>
	/// <param name="filename">ファイル名</param>
	/// <returns></returns>
	ModelData LoadObjFile(const std::string& directoryPath);

	/// <summary>
	/// ハンドルのobjデータのGet
	/// </summary>
	/// <param name="filename">ファイル名</param>
	/// <returns></returns>
	static bool ChackLoadObj(string filePath);

	/// <summary>
	/// mtlファイルの読み込み
	/// </summary>
	/// <param name="directoryPath">ディレクトリ名</param>
	/// <param name="filename">ファイル名</param>
	/// <returns></returns>
	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

private:
	//デバイス
	static ID3D12Device* sDevice_;
	//コマンドリスト
	static ID3D12GraphicsCommandList* sCommandList_;
	//DXC
	static ComPtr<IDxcUtils> sDxcUtils_;
	static ComPtr<IDxcCompiler3> sDxcCompiler_;
	static ComPtr<IDxcIncludeHandler> sIncludeHandler_;
	//RootSignature
	static ComPtr<ID3D12RootSignature> sRootSignature_;
	//PipelineStateObject
	static ComPtr<ID3D12PipelineState> sGraphicsPipelineState_;
	//モデルデータ
	static std::list<ModelData> modelDatas_;
	//頂点データ
	std::unique_ptr<Mesh> mesh_ = nullptr;
	//マテリアルデータ
	std::unique_ptr<Material> material_ = nullptr;
	//DirectionalLight
	std::unique_ptr<DirectionalLight> directionalLight_;
	//テクスチャハンドル
	uint32_t textureHandle_{};

};