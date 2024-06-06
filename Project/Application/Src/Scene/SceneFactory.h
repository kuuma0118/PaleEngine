#pragma once
#include "Engine/Framework/Scene/IntangibleScene.h"
#include "GameTitleScene.h"
#include "GamePlayScene.h"
#include "GameClearScene.h"
#include "GameOverScene.h"
#include "LoadScene.h"

class SceneFactory : public IntangibleScene
{
public:
	IScene* CreateScene(const std::string& sceneName) override;
};

