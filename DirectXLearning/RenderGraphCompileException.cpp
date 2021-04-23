#include "RenderGraphCompileException.h"

#include <sstream>

namespace Rgph {
	RenderGraphCompileException::RenderGraphCompileException(std::string message, int line, const char* file) noexcept
		:
		DirectXException(line, file),
		message(std::move(message))
	{}

	const char* RenderGraphCompileException::what() const noexcept {
		std::ostringstream oss;
		oss << DirectXException::what() << std::endl;
		oss << "[message]" << std::endl;
		oss << message;
		whatBuffer = oss.str();
		return whatBuffer.c_str();
	}

	const char* RenderGraphCompileException::GetType() const noexcept {
		return "Render Graph Compile Error";
	}

	const std::string& RenderGraphCompileException::GetMessage() const noexcept {
		return message;
	}
}