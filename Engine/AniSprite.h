#pragma once

#include "Sprite.h"
#include <string>
#include <vector>

class AniSprite
{
	using Frames = std::vector<grafix::image>;

public:
	AniSprite( const std::string& _base, int _numFrames );
	
	AniSprite( const AniSprite& ) = delete;
	AniSprite& operator=( const AniSprite& ) = delete;

	void Advance( float dt );
	Sprite GetFrame()const;

private:
	Frames frames;
	Frames::const_iterator current;

	const float holdTime = .016f;
	float holdCount = .016f;

};

