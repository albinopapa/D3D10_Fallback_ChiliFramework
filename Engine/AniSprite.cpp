#include "AniSprite.h"



AniSprite::AniSprite( const std::string& _base, int _numFrames )
{
	for( int i = 0; i < _numFrames; ++i )
	{
		std::string filename = _base + std::to_string( i / 10 ) + std::to_string( i % 10 ) + ".png";
		frames.emplace_back( grafix::wic_image_decoder( std::move( filename ) ) );
	}
}

void AniSprite::Advance( float dt ) 
{
	holdCount -= dt;
	if( holdCount <= 0.f )
	{
		holdCount = holdTime;
		++current;
		if( current == frames.end() ) 
		{
			current = frames.begin();
		}
	}
}

Sprite AniSprite::GetFrame()const 
{
	return { *current };
}
