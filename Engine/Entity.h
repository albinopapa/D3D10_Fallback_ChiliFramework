#pragma once

#include "Rect.h"
#include "Matrix.h"

class Graphics;
class World;

class Entity
{
public:
	virtual ~Entity() = default;
	virtual void Update( World& _world, float dt ) = 0;
	virtual void Draw( Graphics& _graphics ) = 0;

	virtual Matrix3x2f GetWorldTransform()const = 0;
	virtual RectF GetRect()const = 0;
};

