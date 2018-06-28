#include "image.h"
#pragma comment(lib,"windowscodecs.lib")

namespace grafix
{
#pragma region GFX_IMAGE
image::image( int _width, int _height, std::unique_ptr<element_type[]> elements )
	:
	_width( _width ),
	_height( _height ),
	pData( std::move( elements ) )
{}

image::image( image&& _src )noexcept
	:
	pData( std::move( _src.pData ) ),
	_width( _src._width ),
	_height( _src._height )
{
	_src._width = 0;
	_src._height = 0;
}

image::~image()
{
	_width = 0;
	_height = 0;
}

image& image::operator=( image&& _src )
{
	if( this != &_src )
	{
		swap( _src );
	}

	return *this;
}

image::element_type& image::operator()( int _xoff, int _yoff )
{
	const auto idx = _xoff + ( _yoff * _width );
	return pData[ idx ];
}

const image::element_type& image::operator()( int _xoff, int _yoff )const
{
	return pData[ _xoff + ( _yoff * _width ) ];
}

void image::clone( const image& _src )
{
	copy_from( _src );
}

int image::stride()const noexcept
{
	return _width * sizeof( element_type );
}

int image::width()const noexcept
{
	return _width;
}

int image::height()const noexcept
{
	return _height;
}

void image::copy_from( const image& _src )
{	
	_width = _src._width;
	_height = _src._height;
	pData = std::make_unique<element_type[]>( _width * _height );

	for( int y = 0; y < _height; ++y )
	{
		auto* dst = &pData[ y * _width ];
		auto const* src = &_src.pData[ y * _width ];
		memcpy( dst, src, stride() );
	}
}

void image::swap( image& src )noexcept
{
	std::swap( *this, src );
}

image::element_type* image::data()noexcept
{
	return pData.get();
}

const image::element_type* image::data()const noexcept
{
	return pData.get();
}

image::element_type* image::get_address_at( int _xoff, int _yoff )
{
	return &pData[ _xoff + ( _yoff * width() ) ];
}

const image::element_type* image::get_address_at( int _xoff, int _yoff )const
{
	return &pData[ _xoff + ( _yoff * width() ) ];
}
#pragma endregion

#pragma region GFX_IMAGE_ECODER
wic_png_image_encoder::wic_png_image_encoder( image& _image, std::string _filename )
{
	auto hr = CoInitialize( nullptr );

	if( FAILED( hr ) )
		throw std::runtime_error( "Failed to initialize the COM library." );

	ComPtr<IWICImagingFactory> factory;
	hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS( factory.GetAddressOf() )
	);
	if( FAILED( hr ) )
		throw std::runtime_error( "Failed to initialize the imaging factory." );

	ComPtr<IWICBitmapEncoder> encoder;
	hr = factory->CreateEncoder( GUID_ContainerFormatPng, nullptr, encoder.GetAddressOf() );
	if( FAILED( hr ) )
		throw std::runtime_error( "Failed to creating the image encoder." );

	ComPtr<IWICStream> stream;
	hr = factory->CreateStream( stream.GetAddressOf() );
	if( FAILED( hr ) )
		throw std::runtime_error( "Failed to creating the file stream." );

	const auto filename = std::wstring( _filename.begin(), _filename.end() );
	hr = stream->InitializeFromFilename( filename.c_str(), GENERIC_WRITE );
	if( FAILED( hr ) )
		throw std::runtime_error( "Failed to initialize the file stream." );

	hr = encoder->Initialize( stream.Get(), WICBitmapEncoderNoCache );
	if( FAILED( hr ) )
		throw std::runtime_error( "Failed to initialize the image encoder." );

	ComPtr<IWICBitmapFrameEncode> frame;
	hr = encoder->CreateNewFrame( frame.GetAddressOf(), nullptr );
	if( FAILED( hr ) )
		throw std::runtime_error( "Failed to create image encoder frame." );

	hr = frame->Initialize( nullptr );
	if( FAILED( hr ) )
		throw std::runtime_error( "Failed to initialize the image encoder frame." );

	const auto width = ( uint32_t )_image.width();
	const auto height = ( uint32_t )_image.height();
	hr = frame->SetSize( width, height );
	if( FAILED( hr ) )
		throw std::runtime_error( "Failed to set the size of the image." );

	auto format = GUID_WICPixelFormat32bppPBGRA;
	hr = frame->SetPixelFormat( &format );
	if( FAILED( hr ) )
		throw std::runtime_error( "Failed to set pixel format of the image." );

	const auto buff_stride = (uint32_t)_image.stride();
	const auto bufferSize = buff_stride * height;
	const auto buffer = reinterpret_cast< WICInProcPointer >( _image.data() );
	hr = frame->WritePixels( height, buff_stride, bufferSize, buffer );
	if( FAILED( hr ) )
		throw std::runtime_error( "Failed to write pixels to the file." );

	hr = frame->Commit();
	if( FAILED( hr ) )
		throw std::runtime_error( "Failed to commit buffer to file." );

	hr = encoder->Commit();
	if( FAILED( hr ) )
		throw std::runtime_error( "Failed to commit buffer to file." );
}

wic_png_image_encoder::~wic_png_image_encoder()
{
	CoUninitialize();
}
#pragma endregion

#pragma region GFX_IMAGE_DECODER
wic_image_decoder::wic_image_decoder( std::string _filename )
{
	auto hr = CoInitialize( nullptr );
	if( SUCCEEDED( hr ) )
	{
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS( wicfactory.GetAddressOf() )
		);
	}
	if( SUCCEEDED( hr ) )
	{
		const auto filename = std::wstring( _filename.begin(), _filename.end() );

		hr = wicfactory->CreateDecoderFromFilename(
			filename.c_str(),
			nullptr,
			GENERIC_READ,
			WICDecodeMetadataCacheOnDemand,
			wicdecoder.GetAddressOf()
		);
	}
	if( SUCCEEDED( hr ) )
	{
		hr = wicdecoder->GetFrame( 0, wicframedecode.GetAddressOf() );
	}

	if( FAILED( hr ) )
		throw std::runtime_error( "Failed to initialize decoder." );
}

int wic_image_decoder::getwidth() const
{
	unsigned int width, height;
	wicframedecode->GetSize( &width, &height );
	return ( int )width;
}

int wic_image_decoder::getheight() const
{
	unsigned int width, height;
	wicframedecode->GetSize( &width, &height );
	return ( int )height;
}

std::unique_ptr<wic_image_decoder::element_type[]> wic_image_decoder::get_image_data() const
{
	const auto width = getwidth();
	const auto height = getheight();
	auto data = std::make_unique<wic_image_decoder::element_type[]>( width * height );

	const unsigned int stride = ( unsigned int )width * ( unsigned int )sizeof( unsigned int );
	const unsigned int buffSize = stride * ( unsigned int )height;
	unsigned char* ptr = reinterpret_cast< unsigned char* >( data.get() );

	ComPtr<IWICFormatConverter> converter;
	auto hr = wicfactory->CreateFormatConverter( converter.GetAddressOf() );

	if( SUCCEEDED( hr ) )
	{
		hr = converter->Initialize(
			wicframedecode.Get(),
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			nullptr,
			1.,
			WICBitmapPaletteTypeCustom
		);
	}

	if( SUCCEEDED( hr ) )
	{
		hr = converter->CopyPixels( nullptr, stride, buffSize, ptr );
	}

	if( FAILED( hr ) )
		throw std::runtime_error( "Failed to decode image." );

	return data;
}

wic_image_decoder::~wic_image_decoder()
{
	wicframedecode = ComPtr<IWICBitmapFrameDecode>();
	wicdecoder = ComPtr<IWICBitmapDecoder>();
	wicfactory = ComPtr<IWICImagingFactory>();
	CoUninitialize();
}
#pragma endregion
}
