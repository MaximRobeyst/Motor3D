#include "DX11Renderer.h"

#include <imgui.h>
#include <backends\imgui_impl_dx11.h>
#include <backends\imgui_impl_win32.h>

#include <io.h>

DX11Renderer::DX11Renderer(HWND hwnd, UINT width, UINT height)
    : Renderer(hwnd, width, height)
{
}

DX11Renderer::~DX11Renderer()
{
#ifdef _DEBUG
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
#endif // _DEBUG

    if (m_pRenderTargetView)
        m_pRenderTargetView->Release();

    if (m_pRenderTargetBuffer)
        m_pRenderTargetBuffer->Release();

    if (m_pDepthStencilView)
        m_pDepthStencilView->Release();

    if (m_pDepthStencilBuffer)
        m_pDepthStencilBuffer->Release();

    if (m_pSwapChain)
        m_pSwapChain->Release();

    if (m_pDeviceContext)
    {
        m_pDeviceContext->ClearState();
        m_pDeviceContext->Flush();
        m_pDeviceContext->Release();
    }

    if (m_pDevice)
        m_pDevice->Release();

}

void DX11Renderer::Initialize()
{
    InitializeDirectX();

#ifdef _DEBUG
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplWin32_Init(m_hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pDeviceContext);
#endif // _DEBUG
}

ID3D11Device* DX11Renderer::GetDevice() const
{
    return m_pDevice;
}

ID3D11DeviceContext* DX11Renderer::GetDeviceContext() const
{
    return m_pDeviceContext;
}

ID3D11DepthStencilView* DX11Renderer::GetStencilView() const
{
    return m_pDepthStencilView;
}

ID3D11RenderTargetView* DX11Renderer::GetRenderTarget() const
{
    return m_pRenderTargetView;
}

IDXGISwapChain* DX11Renderer::GetSwapChain() const
{
    return m_pSwapChain;
}

HRESULT DX11Renderer::InitializeDirectX()
{
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    uint32_t createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT result = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, 0, 0, D3D11_SDK_VERSION, &m_pDevice, &featureLevel, &m_pDeviceContext);
    if (FAILED(result))
        result;

    result = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&m_pDXGIFactory));
    if (FAILED(result))
        result;

    DXGI_SWAP_CHAIN_DESC swapChainDesc{};
    swapChainDesc.BufferDesc.Width = m_Width;
    swapChainDesc.BufferDesc.Height = m_Height;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.Windowed = true;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;

    //Get the handle HWND from the SDL backbuffer
    swapChainDesc.OutputWindow = m_hWnd;

    //Create SwapChain and hook it into the handle of the SDL window
    result = m_pDXGIFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
    if (FAILED(result))
        return result;

    D3D11_TEXTURE2D_DESC depthStencilDesc{};
    depthStencilDesc.Width = m_Width;
    depthStencilDesc.Height = m_Height;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
    depthStencilViewDesc.Format = depthStencilDesc.Format;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    result = m_pDevice->CreateTexture2D(&depthStencilDesc, 0, &m_pDepthStencilBuffer);
    if (FAILED(result))
        return result;

    result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
    if (FAILED(result))
        return result;

    //Create the RenderTargetView
    result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
    if (FAILED(result))
        return result;
    result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, 0, &m_pRenderTargetView);
    if (FAILED(result))
        return result;

    m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

    D3D11_VIEWPORT viewPort{};
    viewPort.Width = static_cast<float>(m_Width);
    viewPort.Height = static_cast<float>(m_Height);
    viewPort.TopLeftX = 0.f;
    viewPort.TopLeftY = 0.f;
    viewPort.MinDepth = 0.f;
    viewPort.MaxDepth = 1.f;
    m_pDeviceContext->RSSetViewports(1, &viewPort);

    return result;
}
