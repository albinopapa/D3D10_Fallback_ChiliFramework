#pragma once

#include "Matrix.h"
#include "Rect.h"
#include "StarState.h"

class Star
{
public:
	Star() = default;
	Star( Vec2f _start_pos, struct SpriteResources& resource, class Keyboard& _keyboard );

	void Update( World& _world, float dt );
	void Draw( class Graphics& _gfx )const;

	void SetPosition( const Vec2f& _position );
	void SetVelocity( const Vec2f& _velocity );

	Matrix<3, 2, float> GetWorld()const;
	RectF GetRect()const;
	const Vec2f& GetPosition()const;
	const Vec2f& GetVelocity()const;
	void SetNextState( std::unique_ptr<StarState> _state );

private:
	friend class StarIdle;
	friend class StarWalk;
	friend class StarJump;

private:
	static constexpr float width = 64.f;
	static constexpr float height = 128.f;
	static constexpr float maxSpeed = 120.f, jumpAccel = 1000.f;
	struct SpriteResources* resource = nullptr;
	class Keyboard* pKeyboard = nullptr;
	Vec2f pos, vel;
	float direction = 1.f;
	std::unique_ptr<StarState> state, next;
};

