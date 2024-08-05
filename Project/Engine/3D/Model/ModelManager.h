#pragma once
#include "Model.h"
#include <filesystem>
#include <unordered_map>
#include <variant>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class ModelManager
{
public:
	static const std::string kBaseDirectory;

	static ModelManager* GetInstance();

	static void Destroy();

	static Model* Create();

	static Model* CreateFromModelFile(const std::string& modelName, DrawPass drawPass);

	void Initialize();

private:
	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(const ModelManager&) = delete;
	ModelManager& operator=(const ModelManager&) = delete;

	Model* CreateInternal(const std::string& modelName, DrawPass drawPass);

	Model::ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);

	SkiningAnimation::Node ReadNode(aiNode* node);

	std::vector<SkiningAnimation::AnimationData> LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

	//Model::ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	//Model::MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

private:
	static ModelManager* instance_;

	std::map<std::string, std::unique_ptr<Model>> models_{};

	std::unordered_map<std::string, std::pair<Model::ModelData, std::vector<SkiningAnimation::AnimationData>>> modelDatas_;
};