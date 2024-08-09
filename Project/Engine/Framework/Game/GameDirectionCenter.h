#pragma once
#include "Engine/Base/WindowAppli.h"
#include "Engine/Base/GraphicsDirectionCenter.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/3D/Lights/LightManager.h"
#include "Engine/Utility/D3DResourceLeakChecker.h"
#include <condition_variable>
#include <mutex>

class GameDirectionCenter
{
public:
	virtual ~GameDirectionCenter() = default;

	virtual void Initialize();

	virtual void Finalize();

	virtual void Update();

	virtual void Draw();

	virtual bool IsEndRequest();

	void Run();

protected:
	D3DResourceLeakChecker leakChecker{};

	WindowAppli* application_ = nullptr;

	GraphicsDirectionCenter* graphicsCore_ = nullptr;

	TextureManager* textureManager_ = nullptr;

	ImGuiManager* imguiManager_ = nullptr;

	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	PostEffects* postEffects_ = nullptr;

	SceneManager* sceneManager_ = nullptr;

	ModelManager* modelManager_ = nullptr;

	LightManager* lightManager_ = nullptr;

	std::unique_ptr<IntangibleScene> sceneFactory_ = nullptr;

	std::unique_ptr<IntangbleObject> gameObjectFactory_ = nullptr;

	std::mutex mutex;

	std::condition_variable condition;

	std::mutex mutex2;

	std::condition_variable condition2;

	bool isLoading_ = false;
};