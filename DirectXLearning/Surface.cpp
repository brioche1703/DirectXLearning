#define FULL_WINTARD

#include "Surface.h"

#include <algorithm>

namespace Gdiplus {
	using std::min;
	using std::max;
}

#include <gdiplus.h>
#include <sstream>
#include <assert.h>

#pragma comment(lib, "gdiplus.lib")

Surface::Surface(unsigned int widht, unsigned int height, unsigned int pitch) noexcept
	:
	pBuffer(std::make_unique<Color[]>(pitch * height)),
	width(widht),
	height(height)
{}

Surface::Surface(unsigned int widht, unsigned int height) noexcept
	:
	Surface(width, height, width)
{}

Surface::Surface(Surface&& source) noexcept
	:
	pBuffer(std::move(source.pBuffer)),
	width(source.width),
	height(source.height)
{}

Surface& Surface::operator=(Surface&& donor) noexcept {
	width = donor.width;
	height = donor.height;
	pBuffer = std::move(donor.pBuffer);
	donor.pBuffer = nullptr;
	return *this;
}

Surface::~Surface() {}


void Surface::Clear(Color fillValue) noexcept {
	memset(pBuffer.get(), fillValue.dword, width * height * sizeof(Color));
}

void Surface::PutPixel(unsigned int x, unsigned int y, Color c) noxnd {
	assert(x >= 0);
	assert(y >= 0);
	assert(x < width);
	assert(y < height);
	pBuffer[y * width + x] = c;
}

Surface::Color Surface::GetPixel(unsigned int x, unsigned int y) const noxnd {
	assert(x >= 0);
	assert(y >= 0);
	assert(x < width);
	assert(y < height);
	return pBuffer[y * width + x];
}


unsigned int Surface::GetWidth() const noexcept {
	return width;
}

unsigned int Surface::GetHeight() const noexcept
{
	return height;
}

Surface::Color* Surface::GetBufferPtr() noexcept
{
	return pBuffer.get();
}

const Surface::Color* Surface::GetBufferPtr() const noexcept
{
	return pBuffer.get();
}

const Surface::Color* Surface::GetBufferPtrConst() const noexcept
{
	return pBuffer.get();
}

Surface Surface::FromFile(const std::string& filename)
{
	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int pitch = 0;

	std::unique_ptr<Color[]> pBuffer = nullptr;

	// Gdiplus needs wide string
	wchar_t wideName[512];
	mbstowcs_s(nullptr, wideName, filename.c_str(), _TRUNCATE);

	Gdiplus::Bitmap bitmap(wideName);
	if (bitmap.GetLastStatus() != Gdiplus::Status::Ok) {
		std::stringstream ss;
		ss << "Loading image [" << filename << "]: Failed to load!";
		throw Exception(__LINE__, __FILE__, ss.str());
	}

	height = bitmap.GetHeight();
	width = bitmap.GetWidth();
	pBuffer = std::make_unique<Color[]>(width * height);

	for (unsigned int y = 0; y < height; y++) {
		for (unsigned int x = 0; x < width; x++) {
			Gdiplus::Color c;
			bitmap.GetPixel(x, y, &c);
			pBuffer[y * width + x] = c.GetValue();
		}
	}

	return Surface(width, height, std::move(pBuffer));
}

void Surface::Save(const std::string& filename) const {
	auto GetEncoderClsid = [&filename](const WCHAR* format, CLSID* pClsid) -> void {
		UINT num = 0;		// number of image encoders
		UINT size = 0;		// size of the image encoder array in bytes

		Gdiplus::ImageCodecInfo* pImageCodeInfo = nullptr;

		Gdiplus::GetImageEncodersSize(&num, &size);
		if (size == 0) {
			std::stringstream ss;
			ss << "Saving surface to [" << filename << "]: Failed to get encoder; size == 0.";
			throw Exception(__LINE__, __FILE__, ss.str());
		}

		pImageCodeInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
		if (pImageCodeInfo == nullptr) {
			std::stringstream ss;
			ss << "Saving surface to [" << filename << "]: Failed to get encoder; Failed to allocate memoryk.";
			throw Exception(__LINE__, __FILE__, ss.str());
		}

		GetImageEncoders(num, size, pImageCodeInfo);

		for (UINT j = 0; j < num; j++) {
			if (wcscmp(pImageCodeInfo[j].MimeType, format) == 0) {
				*pClsid = pImageCodeInfo[j].Clsid;
				free(pImageCodeInfo);
				return;
			}
		}

		free(pImageCodeInfo);

		std::stringstream ss;
		ss << "Saving surface to [" << filename << "]: Failed to get encoder; Failed to find matching encoder.";
		throw Exception(__LINE__, __FILE__, ss.str());
	};

	CLSID bmpID;
	GetEncoderClsid(L"image/bmp", &bmpID);

	wchar_t wideName[512];
	mbstowcs_s(nullptr, wideName, filename.c_str(), _TRUNCATE);

	Gdiplus::Bitmap bitmap(width, height, width * sizeof(Color), PixelFormat32bppARGB, (BYTE*)pBuffer.get());
	if (bitmap.Save(wideName, &bmpID, nullptr) != Gdiplus::Status::Ok) {
		std::stringstream ss;
		ss << "Saving surface to [" << filename << "]: Failed to save!";
		throw Exception(__LINE__, __FILE__, ss.str());

	}
}

void Surface::Copy(const Surface& src) noxnd {
	assert(width == src.width);
	assert(height == src.height);
	memcpy(pBuffer.get(), src.pBuffer.get(), width * height * sizeof(Color));
}

Surface::Surface(unsigned int width, unsigned int height, std::unique_ptr<Color[]> pBufferParam) noexcept
	:
	width(width),
	height(height),
	pBuffer(std::move(pBufferParam))
{}

// Exception
Surface::Exception::Exception(int line, const char* file, std::string note) noexcept
	:
	DirectXException(line, file),
	note(std::move(note))
{}

const char* Surface::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << DirectXException::what() << std::endl
		<< "[Note] " << GetNote();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Surface::Exception::GetType() const noexcept
{
	return "DirectX Graphics Exception";
}

const std::string& Surface::Exception::GetNote() const noexcept
{
	return note;
}
