#pragma once
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Engine/3D/Camera/Camera.h"

class NamedEnemy;

class INamedEnemyState
{
public:
	virtual ~INamedEnemyState() = default;

	virtual void Initialize(NamedEnemy* named) = 0;

	virtual void Update(NamedEnemy* named) = 0;

	virtual void Draw(NamedEnemy* named, const Camera& camera) = 0;

	virtual void OnCollision(Collider* collider) = 0;

	virtual const WorldTransform& GetWorldTransform() const = 0;
};