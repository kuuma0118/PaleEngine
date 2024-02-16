#pragma once
#include "3D/Matrix/ViewProjection.h"

class Muzzle;

class IState {
public:
	virtual ~IState() {};
	virtual void Initialize(Muzzle* pMuzzle) = 0;
	virtual void Update(Muzzle* pMuzzle) = 0;
	virtual void Draw(Muzzle* pMuzzle, const ViewProjection& viewProjection) = 0;
	virtual void ApplyGlobalVariables() = 0;
};