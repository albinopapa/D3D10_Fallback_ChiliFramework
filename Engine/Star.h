#pragma once

#include "Matrix.h"
#include "Rect.h"

class Star
{
public:
	Star() = default;
	Star( Vec2f _start_pos, struct SpriteResources& resource, class Keyboard& _keyboard );

	void Update( float dt );
	void SetPosition( const Vec2f& _position );
	void Draw( class Graphics& _gfx )const;

	Matrix<3, 2, float> GetWorld()const;
	RectF GetRect()const;
	const Vec2f& GetPosition()const;
private:
	static constexpr float maxSpeed = 120.f;
	struct SpriteResources* resource = nullptr;
	class AniSprite* current = nullptr;
	class Keyboard* pKeyboard = nullptr;
	Vec2f pos;
	float direction = 1.f;

};

