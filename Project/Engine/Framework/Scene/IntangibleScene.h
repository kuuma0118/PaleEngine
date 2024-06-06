#pragma once
#include "IScene.h"
#include <string>

class IntangibleScene
{
public:
	virtual ~IntangibleScene() = default;

	virtual IScene* CreateScene(const std::string& sceneName) = 0;
};