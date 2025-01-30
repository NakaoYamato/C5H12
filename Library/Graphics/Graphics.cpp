#include "Graphics.h"

#include <windows.h>
#include <string>

#include "../HRTrace.h"
#include "../ResourceManager/GpuResourceManager.h"

// 初期化処理
void Graphics::Initialize(HWND hwnd, const BOOL FULLSCREEN)
{
	// ハンドル設定
	this->hwnd_ = hwnd;

	// 初期状態のウィンドウのスタイル取得
	windowedStyle_ = static_cast<DWORD>(GetWindowLongPtrW(hwnd, GWL_STYLE));

	if (FULLSCREEN)
	{
		StylizeWindow(TRUE);
	}

#ifdef X3DGP_FULLSCREEN
	if (FULLSCREEN)
	{
		StylizeWindow(TRUE);
	}
#endif

	HRESULT hr = S_OK;

	// 画面サイズ設定
	RECT client_rect;
	GetClientRect(hwnd, &client_rect);
	framebufferDimensions_.cx = client_rect.right - client_rect.left;
	framebufferDimensions_.cy = client_rect.bottom - client_rect.top;

	// デバイス、デバイスコンテキスト、スワップチェーン生成
#ifdef X3DGP_FULLSCREEN
	// デバイス、デバイスコンテキスト、スワップチェーン生成
	{
		// 高性能アダプターの取得
		UINT createFactoryFlags{};
#ifdef _DEBUG
		createFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory6;
		hr = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(dxgiFactory6.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		AcquireHighPerformanceAdapter(dxgiFactory6.Get(), adapter.GetAddressOf());


		UINT createDeviceFlags{};

#if _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		// デバイス、デバイスコンテキストの作成
		D3D_FEATURE_LEVEL featureLevels{ D3D_FEATURE_LEVEL_11_1 };
		hr = D3D11CreateDevice(adapter.Get(),
			D3D_DRIVER_TYPE_UNKNOWN,
			0,
			createDeviceFlags,
			&featureLevels,
			1,
			D3D11_SDK_VERSION,
			&device,
			NULL,
			&immediateContext);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// スワップチェーンの作成
		CreateSwapChain(dxgiFactory6.Get());
	}
#else
	{
		UINT createDeviceFlags{};

#if _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL featureLevels{ D3D_FEATURE_LEVEL_11_0 };

		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = framebufferDimensions_.cx;
		swapChainDesc.BufferDesc.Height = framebufferDimensions_.cy;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = hwnd;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Windowed = !FULLSCREEN;
		hr = D3D11CreateDeviceAndSwapChain(NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			createDeviceFlags,
			&featureLevels,
			1,
			D3D11_SDK_VERSION,
			&swapChainDesc,
			swapChain.GetAddressOf(),
			device.GetAddressOf(),
			NULL,
			immediateContext.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// 深度ステンシルビュー作成
	{
		// 参考資料
		// https://tositeru.github.io/ImasaraDX11//part/ZBuffer-and-depth-stencil
		ID3D11Texture2D* depthStencilBuffer{};
		// 深度ステンシルバッファの作成
		D3D11_TEXTURE2D_DESC texture2dDesc{};
		texture2dDesc.Width = framebufferDimensions_.cx;
		texture2dDesc.Height = framebufferDimensions_.cy;
		texture2dDesc.MipLevels = 1;
		texture2dDesc.ArraySize = 1;
		// 深度値に24bitのfloat型、ステンシル値に8bitのuintを確保している
		texture2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texture2dDesc.SampleDesc.Count = 1;
		texture2dDesc.SampleDesc.Quality = 0;
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		// 深度ステンシルビューを作成するにはリソース作成時に
		// BindFlagsにD3D11_BIND_DEPTH_STENCILを指定
		texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texture2dDesc.CPUAccessFlags = 0;
		texture2dDesc.MiscFlags = 0;
		hr = device->CreateTexture2D(&texture2dDesc, NULL, &depthStencilBuffer);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = texture2dDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		hr = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc,
			depthStencilView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		depthStencilBuffer->Release();
	}

	// レンダーターゲットビュー生成
	{
		ID3D11Texture2D* backBuffer{};
		hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
			reinterpret_cast<LPVOID*>(&backBuffer));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		hr = device->CreateRenderTargetView(backBuffer, NULL, renderTargetView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		backBuffer->Release();
	}

	// ビューポート設定
	{
		D3D11_VIEWPORT viewport{};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = static_cast<float>(framebufferDimensions_.cx);
		viewport.Height = static_cast<float>(framebufferDimensions_.cy);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		immediateContext->RSSetViewports(1, &viewport);
	}
#endif

	// 各管理者の生成
	{
		// 各種ステートの生成
		renderState = std::make_unique<RenderState>(device.Get());
		// 定数バッファの管理者作成
		constantBufferManager = std::make_unique<ConstantBufferManager>(device.Get());
		// ポストエフェクト用の管理者生成
		for (size_t index = 0; index < _countof(frameBufferes); ++index)
		{
			frameBufferes[index] = std::make_unique<FrameBuffer>(device.Get(),
				static_cast<UINT>(GetScreenWidth()),
				static_cast<UINT>(GetScreenHeight()));
		}
		fullscreenQuad = std::make_unique<Sprite>(device.Get(),
			L"",
			".\\Data\\Shader\\FullscreenQuadVS.cso");
		(void)GpuResourceManager::CreatePsFromCso(device.Get(), "./Data/Shader/SpritePS.cso",
			pixelShaders[static_cast<int>(FullscreenQuadPS::EmbeddedPS)].ReleaseAndGetAddressOf());
		(void)GpuResourceManager::CreatePsFromCso(device.Get(), "./Data/Shader/CascadedShadowMapPS.cso",
			pixelShaders[static_cast<int>(FullscreenQuadPS::CascadedPS)].ReleaseAndGetAddressOf());
		(void)GpuResourceManager::CreatePsFromCso(device.Get(), "./Data/Shader/DeferredRenderingPS.cso",
			pixelShaders[static_cast<int>(FullscreenQuadPS::DeferredRenderingPS)].ReleaseAndGetAddressOf());

		cascadedShadowMap = std::make_unique<CascadedShadowMap>(device.Get(),
			1024 * 4, 1024 * 4);

		gBuffer = std::make_unique<GBuffer>(device.Get(), 
			static_cast<UINT>(GetScreenWidth()),
			static_cast<UINT>(GetScreenHeight()),
			static_cast<UINT>(GBufferSRVType::GBufferSRVTypeMAX));
	}
}

// フルスクリーンの切り替え
void Graphics::StylizeWindow(BOOL fullscreen)
{
	fullscreenMode_ = fullscreen;
	if (fullscreen)
	{
		GetWindowRect(hwnd_, &oldWindowRect_);
		SetWindowLongPtrA(hwnd_, GWL_STYLE, WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));

		RECT fullscreen_window_rect;

		HRESULT hr{ E_FAIL };
		if (swapChain)
		{
			Microsoft::WRL::ComPtr<IDXGIOutput> dxgi_output;
			hr = swapChain->GetContainingOutput(&dxgi_output);
			if (hr == S_OK)
			{
				DXGI_OUTPUT_DESC output_desc;
				hr = dxgi_output->GetDesc(&output_desc);
				if (hr == S_OK)
				{
					fullscreen_window_rect = output_desc.DesktopCoordinates;
				}
			}
		}
		if (hr != S_OK)
		{
			DEVMODE devmode = {};
			devmode.dmSize = sizeof(DEVMODE);
			EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);

			fullscreen_window_rect = {
				devmode.dmPosition.x,
				devmode.dmPosition.y,
				devmode.dmPosition.x + static_cast<LONG>(devmode.dmPelsWidth),
				devmode.dmPosition.y + static_cast<LONG>(devmode.dmPelsHeight)
			};
		}
		SetWindowPos(
			hwnd_,
			NULL,
			fullscreen_window_rect.left,
			fullscreen_window_rect.top,
			fullscreen_window_rect.right,
			fullscreen_window_rect.bottom,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);

		ShowWindow(hwnd_, SW_MAXIMIZE);
	}
	else
	{
		SetWindowLongPtrA(hwnd_, GWL_STYLE, windowedStyle_);
		SetWindowPos(
			hwnd_,
			HWND_NOTOPMOST,
			oldWindowRect_.left,
			oldWindowRect_.top,
			oldWindowRect_.right - oldWindowRect_.left,
			oldWindowRect_.bottom - oldWindowRect_.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);

		ShowWindow(hwnd_, SW_NORMAL);
	}
}

// 画面サイズ変更
void Graphics::OnSizeChanged(UINT width, UINT height)
{
	HRESULT hr{ S_OK };
	if (width != framebufferDimensions_.cx || height != framebufferDimensions_.cy)
	{
		framebufferDimensions_.cx = width;
		framebufferDimensions_.cy = height;

		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgi_factory6;
		hr = swapChain->GetParent(IID_PPV_ARGS(dxgi_factory6.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		CreateSwapChain(dxgi_factory6.Get());
	}
}

// バックバッファに描画した画を画面に表示する。
void Graphics::Present(UINT syncInterval)
{
	HRESULT hr{ S_OK };
	UINT flags = (tearingSupported_ && !fullscreenMode_ && syncInterval == 0) ?
		DXGI_PRESENT_ALLOW_TEARING : 0;
	hr = swapChain->Present(syncInterval, flags);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

// PIXEL_SHADER_TYPEでピクセルシェーダを指定して描画
void Graphics::Blit(ID3D11ShaderResourceView** shaderResourceView, 
	uint32_t startSlot, uint32_t numViews, 
	FullscreenQuadPS shaderType)
{
	Graphics::Blit(shaderResourceView,
		startSlot, numViews,
		pixelShaders[static_cast<int>(shaderType)].Get());
}

// この関数の使用者側でピクセルシェーダを指定して描画
void Graphics::Blit(ID3D11ShaderResourceView** shaderResourceView,
	uint32_t startSlot, uint32_t numViews, 
	ID3D11PixelShader* pixelShader)
{
	// 描画処理
	fullscreenQuad->Blit(immediateContext.Get(),
		shaderResourceView,
		startSlot, numViews,
		pixelShader);
}

// 高性能アダプターの取得
void Graphics::AcquireHighPerformanceAdapter(IDXGIFactory6* dxgiFactory6, IDXGIAdapter3** dxgiAdapter3)
{
	HRESULT hr{ S_OK };

	Microsoft::WRL::ComPtr<IDXGIAdapter3> enumeratedAdapter;
	for (UINT adapterIndex = 0;
		DXGI_ERROR_NOT_FOUND != dxgiFactory6->EnumAdapterByGpuPreference(
			adapterIndex,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(enumeratedAdapter.ReleaseAndGetAddressOf()));
		++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 adapterDesc;
		hr = enumeratedAdapter->GetDesc1(&adapterDesc);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// グラフィックボードを検索
		if (adapterDesc.VendorId == 0x1002/*AMD*/ || adapterDesc.VendorId == 0x10DE/*NVIDIA*/)
		{
			// アダプターのデバッグ表示
			//DebugManager::Instance().OutputString(std::wstring(adapterDesc.Description) + L" has been selected.\n");
			//DebugManager::Instance().OutputString(std::string("\tVendorId:" + std::to_string(adapterDesc.VendorId) + '\n'));
			//DebugManager::Instance().OutputString(std::string("\tDeviceId:" + std::to_string(adapterDesc.DeviceId) + '\n'));
			//DebugManager::Instance().OutputString(std::string("\tSubSysId:" + std::to_string(adapterDesc.SubSysId) + '\n'));
			//DebugManager::Instance().OutputString(std::string("\tRevision:" + std::to_string(adapterDesc.Revision) + '\n'));
			//DebugManager::Instance().OutputString(std::string("\tDedicatedVideoMemory:" + std::to_string(adapterDesc.DedicatedVideoMemory) + '\n'));
			//DebugManager::Instance().OutputString(std::string("\tDedicatedSystemMemory:" + std::to_string(adapterDesc.DedicatedSystemMemory) + '\n'));
			//DebugManager::Instance().OutputString(std::string("\tSharedSystemMemory:" + std::to_string(adapterDesc.SharedSystemMemory) + '\n'));
			//DebugManager::Instance().OutputString(std::string("\tAdapterLuid.HighPart:" + std::to_string(adapterDesc.AdapterLuid.HighPart) + '\n'));
			//DebugManager::Instance().OutputString(std::string("\tAdapterLuid.LowPart:" + std::to_string(adapterDesc.AdapterLuid.LowPart) + '\n'));
			//DebugManager::Instance().OutputString(std::string("\tFlags:" + std::to_string(adapterDesc.Flags) + '\n'));
			break;
		}
	}
	// 検索したアダプターを設定
	*dxgiAdapter3 = enumeratedAdapter.Detach();
}

// スワップチェーンの作成
void Graphics::CreateSwapChain(IDXGIFactory6* dxgiFactory6)
{
	HRESULT hr{ S_OK };

	if (swapChain)
	{
		// スワップチェーンの再設定
		ID3D11RenderTargetView* nullRenderTargetView{};
		immediateContext->OMSetRenderTargets(1, &nullRenderTargetView, NULL);
		renderTargetView.Reset();
#if 0
		immediateContext->Flush();
		immediateContext->ClearState();
#endif
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChain->GetDesc(&swapChainDesc);
		hr = swapChain->ResizeBuffers(swapChainDesc.BufferCount, framebufferDimensions_.cx, framebufferDimensions_.cy, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		Microsoft::WRL::ComPtr<ID3D11Texture2D> render_target_buffer;
		hr = swapChain->GetBuffer(0, IID_PPV_ARGS(render_target_buffer.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		D3D11_TEXTURE2D_DESC texture2d_desc;
		render_target_buffer->GetDesc(&texture2d_desc);

		hr = device->CreateRenderTargetView(render_target_buffer.Get(), NULL, renderTargetView.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	else
	{
		BOOL allow_tearing = FALSE;
		if (SUCCEEDED(hr))
		{
			hr = dxgiFactory6->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allow_tearing, sizeof(allow_tearing));
		}
		tearingSupported_ = SUCCEEDED(hr) && allow_tearing;

		// スワップチェーン生成
		DXGI_SWAP_CHAIN_DESC1 swap_chain_desc1{};
		swap_chain_desc1.Width = framebufferDimensions_.cx;
		swap_chain_desc1.Height = framebufferDimensions_.cy;
		swap_chain_desc1.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swap_chain_desc1.Stereo = FALSE;
		swap_chain_desc1.SampleDesc.Count = 1;
		swap_chain_desc1.SampleDesc.Quality = 0;
		swap_chain_desc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap_chain_desc1.BufferCount = 2;
		swap_chain_desc1.Scaling = DXGI_SCALING_STRETCH;
		swap_chain_desc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swap_chain_desc1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swap_chain_desc1.Flags = tearingSupported_ ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
#ifdef X3DGP_FULLSCREEN
		hr = dxgiFactory6->CreateSwapChainForHwnd(device.Get(), hwnd, &swap_chain_desc1, NULL, NULL, swapChain.ReleaseAndGetAddressOf());
#endif
#if 0
		swap_chain_desc1.Flags |= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
#endif
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		hr = dxgiFactory6->MakeWindowAssociation(hwnd_, DXGI_MWA_NO_ALT_ENTER);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		Microsoft::WRL::ComPtr<ID3D11Texture2D> render_target_buffer;
		hr = swapChain->GetBuffer(0, IID_PPV_ARGS(render_target_buffer.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		hr = device->CreateRenderTargetView(render_target_buffer.Get(), NULL, renderTargetView.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// 深度ステンシルビュー作成
	{
		// 参考資料
		// https://tositeru.github.io/ImasaraDX11//part/ZBuffer-and-depth-stencil
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer{};
		// 深度ステンシルバッファの作成
		D3D11_TEXTURE2D_DESC texture2dDesc{};
		texture2dDesc.Width = framebufferDimensions_.cx;
		texture2dDesc.Height = framebufferDimensions_.cy;
		texture2dDesc.MipLevels = 1;
		texture2dDesc.ArraySize = 1;
		// 深度値に24bitのfloat型、ステンシル値に8bitのuintを確保している
		texture2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texture2dDesc.SampleDesc.Count = 1;
		texture2dDesc.SampleDesc.Quality = 0;
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		// 深度ステンシルビューを作成するにはリソース作成時に
		// BindFlagsにD3D11_BIND_DEPTH_STENCILを指定
		texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texture2dDesc.CPUAccessFlags = 0;
		texture2dDesc.MiscFlags = 0;
		hr = device->CreateTexture2D(&texture2dDesc, NULL, depthStencilBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = texture2dDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc,
			depthStencilView.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	}

	// ビューポート設定
	{
		D3D11_VIEWPORT viewport{};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = static_cast<float>(framebufferDimensions_.cx);
		viewport.Height = static_cast<float>(framebufferDimensions_.cy);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		immediateContext->RSSetViewports(1, &viewport);
	}
}
