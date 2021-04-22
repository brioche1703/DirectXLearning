#pragma once

#include "DirectXException.h"

#define RGC_EXCEPTION(message) RenderGraphCompileException(message, __LINE__, __FILE__)

class RenderGraphCompileException : public DirectXException {
public:
	RenderGraphCompileException(std::string name, int line, const char* file) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	const std::string& GetMessage () const noexcept;

private:
	std::string message;
};
