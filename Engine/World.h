#pragma once

#include "FrameTimer.h"
#include "Rect.h"
#include "SpriteResources.h"
#include "Star.h"
#include <functional>
#include <optional>
#include <vector>

class World
{
public:
	World( Keyboard& _keyboard );
	void Update( float dt );
	void Render( Graphics& _graphics )const;
	std::optional<RectF> CollidesWithObject( const RectF& _left );
	bool IsOnGround( const RectF& _position )const;
private:
	// Animations
	SpriteResources resources;

	// Player
	Star star;

	// Obstacle
	std::vector<RectF> obstacles;

	// Ground
	static constexpr float ground = -264.f;

	// Benchtimer
	mutable EventTimer<std::function<void()>> benchtimer;
	int counter = 0;

};

