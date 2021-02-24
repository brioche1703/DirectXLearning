#pragma once

#include "IncludeWin.h"
#include "DirectXException.h"
#include "DxgiInfoManager.h"

#include <d3d11.h>
#include <wrl.h>

class Graphics {
public:
	class Exception : public DirectXException {
		using DirectXException::DirectXException;
	};
	
	class HrException : public Exception {
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;

	private:
		HRESULT hr;
		std::string info;
	};

	class InfoException : public Exception {
	public:
		InfoException(int line, const char* file, std::vector<std::string> info) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};

	class DeviceRemovedException : public HrException {
		using HrException::HrException;

	public:
		const char* GetType() const noexcept override;

	private:
		std::string reason;
	};

public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics() = default;

	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;

	void DrawTriangle();

private:
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif // NDEBUG

	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
};