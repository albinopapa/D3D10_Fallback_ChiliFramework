#pragma once

#include "Vector.h"
#include <memory>

class Keyboard;
class Graphics;
struct SpriteResources;
class Star;
class World;

class StarState
{
public:
	virtual ~StarState() = default;
	virtual void Update( World& _world, Keyboard& _keyboard, float dt ) = 0;
	virtual void Draw( Graphics& _graphics ) = 0;
	virtual const Vec2f& GetAcceleration()const = 0;
};

class StarIdle : public StarState
{
public:
	StarIdle( Star& _star, SpriteResources& _resources );
	void Update( World& _world, Keyboard& _keyboard, float dt )override;
	void Draw( Graphics& _graphics )override;
	const Vec2f& GetAcceleration()const override;

private:
	Star & star;
	SpriteResources& resources;
	Vec2f acceleration = { 0.f, 0.f };
};

class StarWalk : public StarState
{
public:
	StarWalk( Star& _star, SpriteResources& _resources );
	void Update( World& _world, Keyboard& _keyboard, float dt )override;
	void Draw( Graphics& _graphics )override;
	const Vec2f& GetAcceleration()const override;

private:
	Star & star;
	SpriteResources& resources;
	Vec2f acceleration = { 0.f, 0.f };
};

class StarJump : public StarState
{
public:
	StarJump( Star& _star, SpriteResources& _resources );
	void Update( World& _world, Keyboard& _keyboard, float dt )override;
	void Draw( Graphics& _graphics )override;
	const Vec2f& GetAcceleration()const override;

private:
	Star & star;
	SpriteResources& resources;
	Vec2f acceleration = { 0.f, 0.f };
};