#pragma once
#include "Engine/Framework/Object/IntangibleObject.h"
#include "Games/GameObj/Player/Player.h"
#include "Games/GameObj/NamedEnemy/NamedEnemy.h"


class CreateGameObj : public IntangbleObject
{
public:
	IGameObject* CreateGameObject(const std::string& objectName) override;
};