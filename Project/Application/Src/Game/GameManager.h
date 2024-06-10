#pragma once
#include "Engine/Framework/Game/GameDirectionCenter.h"
#include "Application/Src/Scene/SceneFactory.h"

class GameManager : public GameDirectionCenter
{
public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

private:

};

