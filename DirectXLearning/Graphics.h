#pragma once

#include "IncludeWin.h"
#include "DirectXException.h"
#include "DxgiInfoManager.h"
#include "ConditionalNoexcept.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include "CustomWRL.h"
#include <DirectXMath.h>
#include <memory>

namespace Bind {
	class Bindable;
	class RenderTarget;
	class OutputOnlyDepthStencil;
}

class Graphics {
	friend class GraphicsResource;
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
	Graphics(HWND hWnd, int width, int height);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics() = default;

	void BeginFrame(float red, float green, float blue) noexcept;
	void EndFrame();

	void DrawIndexed(UINT count) noxnd;
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;

	void SetCamera(DirectX::FXMMATRIX cam) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;

	void EnableImgui() noexcept;
	void DisableImgui() noexcept;
	bool IsImguiEnabled() const noexcept;

	UINT GetWidth() const noexcept;
	UINT GetHeight() const noexcept;
	void SetWidth(UINT width) noexcept;
	void SetHeight(UINT height) noexcept;
	void Resize(UINT width, UINT height) noexcept;
	std::string GetGpuName() const noexcept;
	std::shared_ptr<Bind::RenderTarget> GetTarget();
	std::shared_ptr<Bind::OutputOnlyDepthStencil> GetDepthStencil();

private:
	std::string GetGpuName_() const noexcept;

private:
	UINT width;
	UINT height;
	std::string gpuName;
	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX camera;
	bool imguiEnabled = true;

#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif // NDEBUG

	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	std::shared_ptr<Bind::RenderTarget> pTarget;
	std::shared_ptr<Bind::OutputOnlyDepthStencil> pDepthStencil;
};
