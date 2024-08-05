#pragma once
#include "IGameObject.h"
#include <string>

class IntangbleObject
{
public:
	virtual ~IntangbleObject() = default;

	virtual IGameObject* CreateGameObject(const std::string& objectName) = 0;
};