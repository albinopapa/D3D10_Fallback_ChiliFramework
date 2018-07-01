#pragma once

#include "AniSprite.h"
#include "utility.h"

struct SpriteResources
{
public:
	SpriteResources()
		:
		star_idle( "sprite/idle", 1 ),
		star_walk( "sprite/walk", 30 ),
		box( 32, 32, std::make_unique<grafix::image::element_type[]>( 32 * 32 ) )
	{
		for_each( { 0, 0, 32, 32 }, [ this ]( int x, int y ) { box( x, y ) = Color( Colors::Green, 255 ).dword; } );
	}

public:
	AniSprite star_idle, star_walk;
	grafix::image box;
};