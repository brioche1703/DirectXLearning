#include "Graphics.h"
#include "GraphicsThrowMacros.h"
#include "DepthStencil.h"
#include "Bindable.h"
#include "BindableCommon.h"
#include "RenderTarget.h"


#include <sstream>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <array>
#include <dxgi.h>

#include "dxerr.h"
#include "external/imgui/imgui_impl_dx11.h"
#include "external/imgui/imgui_impl_win32.h"

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")
#pragma comment(lib,"dxgi.lib")



Graphics::Graphics(HWND hWnd, int width, int height) 
	:
	width(width),
	height(height)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	UINT swapChainCreateFlags = 0u;
#ifndef NDEBUG
	swapChainCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // !NDEBUG
	
	HRESULT hr;
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0u | D3D11_CREATE_DEVICE_DEBUG,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwapChain,
		&pDevice,
		nullptr,
		&pContext));

	// Get access to texture subresource in swap chain (back buffer)
	wrl::ComPtr<ID3D11Texture2D> pBackBuffer;
	GFX_THROW_INFO(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer));
	pTarget = std::shared_ptr<Bind::RenderTarget>{ new Bind::OutputOnlyRenderTarget(*this, pBackBuffer.Get()) };

	D3D11_VIEWPORT vp;
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pContext->RSSetViewports(1u, &vp);

	ImGui_ImplDX11_Init(pDevice.Get(), pContext.Get());
	gpuName = GetGpuName_();
}

void Graphics::DrawIndexed(UINT count) noxnd {
	GFX_THROW_INFO_ONLY(pContext->DrawIndexed(count, 0u, 0u));
}

void Graphics::SetProjection(DirectX::FXMMATRIX proj) noexcept {
	projection = proj;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept {
	return projection;
}

void Graphics::SetCamera(DirectX::FXMMATRIX cam) noexcept {
	camera = cam;
}

DirectX::XMMATRIX Graphics::GetCamera() const noexcept
{
	return camera;
}

void Graphics::EnableImgui() noexcept {
	imguiEnabled = true;
}

void Graphics::DisableImgui() noexcept {
	imguiEnabled = false;
}

bool Graphics::IsImguiEnabled() const noexcept {
	return imguiEnabled;
}

UINT Graphics::GetWidth() const noexcept {
	return width;
}
UINT Graphics::GetHeight() const noexcept {
	return height;
}

std::string Graphics::GetGpuName() const noexcept {
	return gpuName;
}

std::string Graphics::GetGpuName_() const noexcept {
	wrl::ComPtr<IDXGIFactory1> dxgiFactory;
	CreateDXGIFactory1(IID_PPV_ARGS(dxgiFactory.ReleaseAndGetAddressOf()));
	wrl::ComPtr<IDXGIAdapter1> adapter;

	std::wstring gpuName;
	dxgiFactory->EnumAdapters1(0, adapter.ReleaseAndGetAddressOf());
	DXGI_ADAPTER_DESC1 desc = {};
	adapter->GetDesc1(&desc);
	gpuName = desc.Description;

	return std::string(gpuName.begin(), gpuName.end());
}

std::shared_ptr<Bind::RenderTarget> Graphics::GetTarget() {
	return pTarget;
}

void Graphics::BeginFrame(float red, float green, float blue) noexcept
{
	if (imguiEnabled)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
	ID3D11ShaderResourceView* const pNullTex = nullptr;
	pContext->PSSetShaderResources(0, 1, &pNullTex);
	pContext->PSSetShaderResources(3, 1, &pNullTex);
}

void Graphics::EndFrame()
{
	if (imguiEnabled)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif // !NDEBUG
	if (FAILED(hr = pSwapChain->Present(1u, 0u))) {
		if (hr == DXGI_ERROR_DEVICE_REMOVED) {
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else {
			throw GFX_EXCEPT(hr);
		}
	}
	pSwapChain->Present(1u, 0u);
}

Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	: Exception(line, file), hr(hr) {
	for (const auto& msg : infoMsgs) {
		info += msg;
		info.push_back('\n');
	}
	if (!info.empty()) {
		info.pop_back();
	}
}

const char* Graphics::HrException::what() const noexcept { 
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;

	if (!info.empty()) {
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept {
	return "DirectX Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept {
	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept {
	return DXGetErrorString(hr);
}

std::string Graphics::HrException::GetErrorDescription() const noexcept {
	char buf[512];
	DXGetErrorDescription(hr, buf, sizeof(buf));
	return buf;
}

std::string Graphics::HrException::GetErrorInfo() const noexcept {
	return info;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept {
	return "DirectX Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept
	: Exception(line, file) {
	for (const auto& m : infoMsgs) {
		info += m;
		info.push_back('\n');
	}
	if (!info.empty()) {
		info.pop_back();
	}
}

const char* Graphics::InfoException::what() const noexcept { 
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept {
	return "DirectX Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept {
	return info;
}
