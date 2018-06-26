#pragma once

#include "ChiliWin.h"
#include "ChiliException.h"
#include <wrl/client.h>
#include "Colors.h"

class D3DBackend
{
public:
	class Exception : public ChiliException
	{
	public:
		Exception( HRESULT hr, const std::wstring& note, const wchar_t* file, unsigned int line );

		std::wstring GetErrorName() const;
		std::wstring GetErrorDescription() const;
		std::wstring GetFullMessage() const override;
		std::wstring GetExceptionType() const override;

	private:
		HRESULT hr;
	};

public:
	virtual ~D3DBackend() = default;

	virtual void Present( const Color* pSysBuffer ) = 0;

protected:
	// vertex format for the framebuffer fullscreen textured quad
	struct FSQVertex
	{
		float x, y, z;		// position
		float u, v;			// texcoords
	};

};

