#pragma once

/*
Image defines a 2D buffer with width and height defining it's extents
*/

#include "ChiliWin.h"
#include <wincodec.h>
#include <string>
#include <memory>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace grafix
{
class wic_image_decoder
{
public:
	using element_type = unsigned int;

public:
	wic_image_decoder() = default;
	wic_image_decoder( std::string _filename );
	int getwidth()const;
	int getheight()const;
	std::unique_ptr<element_type[]> get_image_data()const;
	~wic_image_decoder();

private:
	ComPtr<IWICImagingFactory> wicfactory;
	ComPtr<IWICBitmapDecoder> wicdecoder;
	ComPtr<IWICBitmapFrameDecode> wicframedecode;
	
};

class wic_png_image_encoder
{
public:
	wic_png_image_encoder() = default;
	wic_png_image_encoder( class image& _image, std::string _filename );
	~wic_png_image_encoder();
};

class image
{
public:
	using element_type = unsigned int;

public:
	image() = default;
	template<class decoder_t> image( const decoder_t& _decoder )
		:
		image( _decoder.getwidth(), _decoder.getheight(), _decoder.get_image_data() )
	{}
	image( int _width, int _height, std::unique_ptr<element_type[]> elements );
	image( const image& ) = delete;
	image( image&& _src )noexcept;
	~image();

	image& operator=( const image& ) = delete;
	image& operator=( image&& _src );

	element_type& operator()( int _xoff, int _yoff );
	const element_type& operator()( int _xoff, int _yoff )const;

	void clone( const image& _src );
	int stride()const noexcept;
	int width()const noexcept;
	int height()const noexcept;
	element_type* data()noexcept;
	const element_type* data()const noexcept;
	element_type* get_address_at( int _xoff, int _yoff );
	const element_type* get_address_at( int _xoff, int _yoff )const;

private:
	void copy_from( const image& _src );
	void swap( image& _src )noexcept;

private:
	int _width, _height;	
	std::unique_ptr<element_type[]> pData;
};

}
