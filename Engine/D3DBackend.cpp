#include "D3DBackend.h"
#include "DXErr.h"
#include <array>


D3DBackend::Exception::Exception( HRESULT hr, const std::wstring& note, const wchar_t* file, unsigned int line )
	:
	ChiliException( file, line, note ),
	hr( hr )
{}

std::wstring D3DBackend::Exception::GetFullMessage() const
{
	const std::wstring empty = L"";
	const std::wstring errorName = GetErrorName();
	const std::wstring errorDesc = GetErrorDescription();
	const std::wstring& note = GetNote();
	const std::wstring location = GetLocation();
	return    ( !errorName.empty() ? std::wstring( L"Error: " ) + errorName + L"\n"
		: empty )
		+ ( !errorDesc.empty() ? std::wstring( L"Description: " ) + errorDesc + L"\n"
			: empty )
		+ ( !note.empty() ? std::wstring( L"Note: " ) + note + L"\n"
			: empty )
		+ ( !location.empty() ? std::wstring( L"Location: " ) + location
			: empty );
}

std::wstring D3DBackend::Exception::GetErrorName() const
{
	return DXGetErrorString( hr );
}

std::wstring D3DBackend::Exception::GetErrorDescription() const
{
	std::array<wchar_t, 512> wideDescription;
	DXGetErrorDescription( hr, wideDescription.data(), wideDescription.size() );
	return wideDescription.data();
}

std::wstring D3DBackend::Exception::GetExceptionType() const
{
	return L"Chili Graphics Exception";
}

