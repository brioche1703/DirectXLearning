#include "DirectXException.h"
#include <sstream>

DirectXException::DirectXException(int line, const char* file) noexcept
	: 
	line(line),
	file(file) {}

const char* DirectXException::what() const noexcept {
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* DirectXException::GetType() const noexcept
{
	return "DirectX Exception";
}

int DirectXException::GetLine() const noexcept
{
	return line;
}

const std::string& DirectXException::GetFile() const noexcept
{
	return file;
}

std::string DirectXException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return oss.str();
}
