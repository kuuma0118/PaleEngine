#include "ModelManager.h"
#include "Engine/Math/MathFunction.h"
#include <cassert>
#include <fstream>
#include <sstream>

//実体定義
ModelManager* ModelManager::instance_ = nullptr;
const std::string ModelManager::kBaseDirectory = "Resources/Models";

ModelManager* ModelManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new ModelManager();
	}
	return instance_;
}

void ModelManager::Destroy()
{
	if (instance_)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

Model* ModelManager::Create()
{
	Model* model = ModelManager::GetInstance()->CreateInternal("Cube.obj", Opaque);
	return model;
}

Model* ModelManager::CreateFromModelFile(const std::string& modelName, DrawPass drawPass)
{
	Model* model = ModelManager::GetInstance()->CreateInternal(modelName, drawPass);
	return model;
}

Model* ModelManager::CreateInternal(const std::string& modelName,DrawPass drawPass)
{
	auto it = modelDatas_.find(modelName);

	if (it != modelDatas_.end())
	{
		Model* model = new Model();
		model->Create(std::get<Model::ModelData>(it->second), std::get<Model::Animation>(it->second), drawPass);
		return model;
	}

	//モデルデータの読み込み
	size_t extensionIndex = modelName.find_last_of('.');
	std::string directoryPath = kBaseDirectory + "/" + modelName.substr(0, extensionIndex);
	Model::ModelData modelData = LoadModelFile(directoryPath, modelName);
	modelDatas_[modelName] = modelData;
	Model::Animation animationData = LoadAnimationFile(directoryPath, modelName);
	modelDatas_[modelName] = animationData;

	//モデルの生成
	Model* model = new Model();
	model->Create(modelData, animationData, drawPass);

	return model;
}

void ModelManager::Initialize()
{
	Model::ModelData modelData = LoadModelFile("Resources/Models/Cube", "Cube.obj");
	modelDatas_["Cube.obj"] = modelData;
}


Model::ModelData ModelManager::LoadModelFile(const std::string& directoryPath, const std::string& filename)
{
	Model::ModelData modelData;
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());//メッシュがないのは対応しない

	//Meshの解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
	{
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());//法線がないMeshは今回は非対応
		assert(mesh->HasTextureCoords(0));//TexcoordがないMeshは今回は非対応
		modelData.vertices.resize(mesh->mNumVertices);//最初に頂点数分のメモリを確保しておく
		//ここからMeshの中身(Face)の解析を行っていく
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
		{
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);//三角形のみサポート
			//ここからFaceの中身(Vertex)の解析を行っていく
			for (uint32_t element = 0; element < face.mNumIndices; ++element)
			{
				uint32_t vertexIndex = face.mIndices[element];
				aiVector3D& position = mesh->mVertices[vertexIndex];
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				//右手系->左手系への変換を忘れずに
				modelData.vertices[vertexIndex].position = { -position.x,position.y,position.z,1.0f };
				modelData.vertices[vertexIndex].normal = { -normal.x,normal.y,normal.z };
				modelData.vertices[vertexIndex].texcoord = { texcoord.x,texcoord.y };
			}
			//Indexの解析
			for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
			{
				aiFace& face = mesh->mFaces[faceIndex];
				assert(face.mNumIndices == 3);

				for (uint32_t element = 0; element < face.mNumIndices; ++element)
				{
					uint32_t vertexIndex = face.mIndices[element];
					modelData.indices.push_back(vertexIndex);
				}
			}
			//SkinCluster構築用のデータを取得
			for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
			{
				aiBone* bone = mesh->mBones[boneIndex];
				std::string jointName = bone->mName.C_Str();
				Model::JointWeightData& jointWeightData = modelData.skinClusterData[jointName];

				aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();//BindPoseMatrixに戻す
				aiVector3D scale, translate;
				aiQuaternion rotate;
				bindPoseMatrixAssimp.Decompose(scale, rotate, translate);//成分を抽出
				//左手系のBindPoseMatrixを作る
				Matrix4x4 bindPoseMatrix = Mathf::MakeAffineMatrix({ scale.x,scale.y,scale.z },
					{ rotate.x,-rotate.y,-rotate.z,rotate.w },
					{ -translate.x,translate.y,translate.z });
				//InverseBindPoseMatrixにする
				jointWeightData.inverseBindPoseMatrix = Mathf::Inverse(bindPoseMatrix);
				//Weight情報を取り出す
				for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex)
				{
					jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight,bone->mWeights[weightIndex].mVertexId });
				}
			}
		}
	}

	//Materialの解析
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex)
	{
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0)
		{
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.material.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
		}
	}

	//Nodeの解析
	modelData.rootNode = ReadNode(scene->mRootNode);

	return modelData;
}

Model::Node ModelManager::ReadNode(aiNode* node)
{
	Model::Node result{};
	aiVector3D scale, translate;
	aiQuaternion rotate;
	node->mTransformation.Decompose(scale, rotate, translate);//assimpの行列からSRTを抽出する関数を利用
	result.scale = { scale.x,scale.y,scale.z };//Scaleはそのまま
	result.rotate = { rotate.x,-rotate.y,-rotate.z,rotate.w };//X軸を反転、さらに回転方向が逆なので軸を反転させる
	result.translate = { -translate.x,translate.y,translate.z };//X軸を反転
	result.localMatrix = Mathf::MakeAffineMatrix(result.scale, result.rotate, result.translate);
	result.name = node->mName.C_Str();//Node名を格納
	result.children.resize(node->mNumChildren);//子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
	{
		//再帰的に読んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}
	return result;
}

Model::Animation ModelManager::LoadAnimationFile(const std::string& directoryPath, const std::string& filename)
{
	Model::Animation animation{};//今回作るアニメーション
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
	assert(scene->mAnimations != 0);//アニメーションがない
	aiAnimation* animationAssimp = scene->mAnimations[0];//最初のアニメーションだけ採用。もちろん複数対応することに越したことはない
	animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);//時間の単位を秒に変換
	//assimpでは個々のNodeのAnimationをchannelと読んでいるのでchannelを回してNodeAnimationの情報を取ってくる
	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex)
	{
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		Model::NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];
		//Translate
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex)
		{
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			Model::KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);//ここも秒に変換
			keyframe.value = { -keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };//右手->左手
			nodeAnimation.translate.keyframes.push_back(keyframe);
		}
		//Rotate
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex)
		{
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			Model::KeyframeQuaternion keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { keyAssimp.mValue.x,-keyAssimp.mValue.y,-keyAssimp.mValue.z,keyAssimp.mValue.w };
			nodeAnimation.rotate.keyframes.push_back(keyframe);
		}
		//Scale
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex)
		{
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			Model::KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };
			nodeAnimation.scale.keyframes.push_back(keyframe);
		}
	}

	//解析完了
	return animation;
}