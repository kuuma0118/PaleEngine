#include "CreateGameObj.h"

IGameObject* CreateGameObj::CreateGameObject(const std::string& objectName)
{
	if (objectName == "Player")
	{
		Player* player = new Player();
		return player;
	}
	else if (objectName == "NamedEnemy")
	{
		NamedEnemy* namedEnemy = new NamedEnemy();
		return namedEnemy;
	}
	
	return nullptr;
}