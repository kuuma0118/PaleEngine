#pragma once
#include "IScene.h"
#include <string>

class ParticularScene
{
public:
	virtual ~ParticularScene() = default;

	virtual IScene* CreateScene(const std::string& sceneName) = 0;
};