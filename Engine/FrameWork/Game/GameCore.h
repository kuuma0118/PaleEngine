#pragma once
#include "Base/WindowAppli.h"
#include "Base/GraphicsCore.h"
#include "Base/TextureManager.h"
#include "Base/ImGuiManager.h"
#include "Base/Renderer.h"
#include "Components/Input.h"
#include "Components/Audio.h"
#include "3D/Particle/ParticleManager.h"
#include "Components/PostProcess.h"
#include "Framework/Scene/SceneManager.h"
#include "Framework/Object/GameObjectManager.h"
#include "3D/Model/ModelManager.h"
#include "3D/Light/LightManager.h"
#include "Utility/D3DResourceLeakChecker.h"
#include <condition_variable>
#include <mutex>

class GameCore
{
public:
	virtual ~GameCore() = default;

	virtual void Initialize();

	virtual void Finalize();

	virtual void Update();

	virtual void Draw();

	virtual bool IsEndRequest();

	void Run();

protected:
	D3DResourceLeakChecker leakChecker{};

	WindowAppli* application_ = nullptr;

	GraphicsCore* graphicsCore_ = nullptr;

	TextureManager* textureManager_ = nullptr;

	ImGuiManager* imguiManager_ = nullptr;

	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	PostProcess* postEffects_ = nullptr;

	SceneManager* sceneManager_ = nullptr;

	ModelManager* modelManager_ = nullptr;

	LightManager* lightManager_ = nullptr;

	std::unique_ptr<ParticularScene> sceneFactory_ = nullptr;

	std::mutex mutex;

	std::condition_variable condition;

	std::mutex mutex2;

	std::condition_variable condition2;

	bool isLoading_ = false;
};

