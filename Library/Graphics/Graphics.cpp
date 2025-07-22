#include "Graphics.h"

#include <windows.h>
#include <string>

#include "../HRTrace.h"
#include "GpuResourceManager.h"
#include "../DebugSupporter/DebugSupporter.h"

#include <imgui.h>

// 初期化処理
void Graphics::Initialize(HWND hwnd, HINSTANCE instance, const BOOL FULLSCREEN)
{
	// ハンドル設定
	this->_hwnd = hwnd;
	// インスタンス設定
	this->_hInstance = instance;

	// 初期状態のウィンドウのスタイル取得
	_windowedStyle = static_cast<DWORD>(GetWindowLongPtrW(hwnd, GWL_STYLE));

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
	_framebufferDimensions.cx = client_rect.right - client_rect.left;
	_framebufferDimensions.cy = client_rect.bottom - client_rect.top;

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
		swapChainDesc.BufferDesc.Width = _framebufferDimensions.cx;
		swapChainDesc.BufferDesc.Height = _framebufferDimensions.cy;
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
			_swapChain.GetAddressOf(),
			_device.GetAddressOf(),
			NULL,
			_immediateContext.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// 深度ステンシルビュー作成
	{
		// 参考資料
		// https://tositeru.github.io/ImasaraDX11//part/ZBuffer-and-depth-stencil
		ID3D11Texture2D* depthStencilBuffer{};
		// 深度ステンシルバッファの作成
		D3D11_TEXTURE2D_DESC texture2dDesc{};
		texture2dDesc.Width = _framebufferDimensions.cx;
		texture2dDesc.Height = _framebufferDimensions.cy;
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
		hr = _device->CreateTexture2D(&texture2dDesc, NULL, &depthStencilBuffer);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = texture2dDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		hr = _device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc,
			_depthStencilView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		depthStencilBuffer->Release();
	}

	// レンダーターゲットビュー生成
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetBuffer;
		hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
			reinterpret_cast<LPVOID*>(renderTargetBuffer.ReleaseAndGetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		hr = _device->CreateRenderTargetView(renderTargetBuffer.Get(), NULL, _renderTargetView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// ビューポート設定
	{
		D3D11_VIEWPORT viewport{};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = static_cast<float>(_framebufferDimensions.cx);
		viewport.Height = static_cast<float>(_framebufferDimensions.cy);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		_immediateContext->RSSetViewports(1, &viewport);
	}
#endif

	// 各管理者の生成
	{
		UINT width = static_cast<UINT>(GetScreenWidth());
		UINT height = static_cast<UINT>(GetScreenHeight());
		// 各種ステートの生成
		_renderState = std::make_unique<RenderState>(_device.Get());
		// 定数バッファの管理者作成
		_constantBufferManager = std::make_unique<ConstantBufferManager>(_device.Get());
		// ポストエフェクト用の管理者生成
		for (size_t index = 0; index < _countof(_frameBufferes); ++index)
		{
			_frameBufferes[index] = std::make_unique<FrameBuffer>(_device.Get(), width, height);
		}
		// カスケードシャドウマップ管理者生成
		_cascadedShadowMap = std::make_unique<CascadedShadowMap>(_device.Get(),
			1024 * 6, 1024 * 6);
		// GBuffer生成
		_gBuffer = std::make_unique<GBuffer>(_device.Get(), width, height);
	}
}

// フルスクリーンの切り替え
void Graphics::StylizeWindow(BOOL fullscreen)
{
	_fullscreenMode = fullscreen;
	if (fullscreen)
	{
		GetWindowRect(_hwnd, &_oldWindowRect);
		SetWindowLongPtrA(_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));

		RECT fullscreen_window_rect;

		HRESULT hr{ E_FAIL };
		if (_swapChain)
		{
			Microsoft::WRL::ComPtr<IDXGIOutput> dxgi_output;
			hr = _swapChain->GetContainingOutput(&dxgi_output);
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
			_hwnd,
			NULL,
			fullscreen_window_rect.left,
			fullscreen_window_rect.top,
			fullscreen_window_rect.right,
			fullscreen_window_rect.bottom,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);

		ShowWindow(_hwnd, SW_MAXIMIZE);
	}
	else
	{
		SetWindowLongPtrA(_hwnd, GWL_STYLE, _windowedStyle);
		SetWindowPos(
			_hwnd,
			HWND_NOTOPMOST,
			_oldWindowRect.left,
			_oldWindowRect.top,
			_oldWindowRect.right - _oldWindowRect.left,
			_oldWindowRect.bottom - _oldWindowRect.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);

		ShowWindow(_hwnd, SW_NORMAL);
	}
}

// 画面サイズ変更
void Graphics::OnSizeChanged(UINT width, UINT height)
{
	HRESULT hr{ S_OK };
	if (width != _framebufferDimensions.cx || height != _framebufferDimensions.cy)
	{
		_framebufferDimensions.cx = width;
		_framebufferDimensions.cy = height;

		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgi_factory6;
		hr = _swapChain->GetParent(IID_PPV_ARGS(dxgi_factory6.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		CreateSwapChain(dxgi_factory6.Get());
	}
}

// バックバッファに描画した画を画面に表示する。
void Graphics::Present(UINT syncInterval)
{
	HRESULT hr{ S_OK };
	UINT flags = (_tearingSupported && !_fullscreenMode && syncInterval == 0) ?
		DXGI_PRESENT_ALLOW_TEARING : 0;
	hr = _swapChain->Present(syncInterval, flags);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

// ImGui描画
void Graphics::DrawGui()
{
#if USE_IMGUI
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu(u8"デバッグ"))
		{
			if (ImGui::BeginMenu(u8"描画管理"))
			{
				ImGui::Checkbox(u8"フレームバッファ", &_drawFrameBuffer);
				ImGui::Checkbox(u8"デファードレンダリング", &_renderingDeferred);
				ImGui::Checkbox(u8"シャドウマップ", &_drawCSMGui);
				ImGui::Checkbox(u8"GBuffer", &_drawGBGui);

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
	// フレームバッファGUI
	if (_drawFrameBuffer)
	{
		if (ImGui::Begin(u8"フレームバッファ"))
		{
			static float textureSize = 256.0f;
			ImGui::DragFloat("TextureSize", &textureSize);
			for (UINT bufferIndex = 0; bufferIndex < 4; ++bufferIndex)
			{
				ImGui::Text(u8"%d", bufferIndex);
				ImGui::Image(_frameBufferes[bufferIndex]->GetColorSRV().Get(),
					{ textureSize ,textureSize });
				ImGui::SameLine();
				ImGui::Image(_frameBufferes[bufferIndex]->GetDepthSRV().Get(),
					{ textureSize ,textureSize });
			}
		}
		ImGui::End();
	}

	// カスケードシャドウマップGUI
	if (_drawCSMGui)
		GetCascadedShadowMap()->DrawGui();

	// GBufferGUI
	if (_drawGBGui)
		GetGBuffer()->DrawGui();
#endif
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
			Debug::Output::String(std::wstring(adapterDesc.Description) + L" has been selected.\n");
			Debug::Output::String(std::string("\tVendorId:" + std::to_string(adapterDesc.VendorId) + '\n'));
			Debug::Output::String(std::string("\tDeviceId:" + std::to_string(adapterDesc.DeviceId) + '\n'));
			Debug::Output::String(std::string("\tSubSysId:" + std::to_string(adapterDesc.SubSysId) + '\n'));
			Debug::Output::String(std::string("\tRevision:" + std::to_string(adapterDesc.Revision) + '\n'));
			Debug::Output::String(std::string("\tDedicatedVideoMemory:" + std::to_string(adapterDesc.DedicatedVideoMemory) + '\n'));
			Debug::Output::String(std::string("\tDedicatedSystemMemory:" + std::to_string(adapterDesc.DedicatedSystemMemory) + '\n'));
			Debug::Output::String(std::string("\tSharedSystemMemory:" + std::to_string(adapterDesc.SharedSystemMemory) + '\n'));
			Debug::Output::String(std::string("\tAdapterLuid.HighPart:" + std::to_string(adapterDesc.AdapterLuid.HighPart) + '\n'));
			Debug::Output::String(std::string("\tAdapterLuid.LowPart:" + std::to_string(adapterDesc.AdapterLuid.LowPart) + '\n'));
			Debug::Output::String(std::string("\tFlags:" + std::to_string(adapterDesc.Flags) + '\n'));
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

	if (_swapChain)
	{
		// スワップチェーンの再設定
		ID3D11RenderTargetView* nullRenderTargetView{};
		_immediateContext->OMSetRenderTargets(1, &nullRenderTargetView, NULL);
		_renderTargetView.Reset();
#if 0
		immediateContext->Flush();
		immediateContext->ClearState();
#endif
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		_swapChain->GetDesc(&swapChainDesc);
		hr = _swapChain->ResizeBuffers(swapChainDesc.BufferCount, _framebufferDimensions.cx, _framebufferDimensions.cy, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		Microsoft::WRL::ComPtr<ID3D11Texture2D> render_target_buffer;
		hr = _swapChain->GetBuffer(0, IID_PPV_ARGS(render_target_buffer.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		D3D11_TEXTURE2D_DESC texture2d_desc;
		render_target_buffer->GetDesc(&texture2d_desc);

		hr = _device->CreateRenderTargetView(render_target_buffer.Get(), NULL, _renderTargetView.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	else
	{
		BOOL allow_tearing = FALSE;
		if (SUCCEEDED(hr))
		{
			hr = dxgiFactory6->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allow_tearing, sizeof(allow_tearing));
		}
		_tearingSupported = SUCCEEDED(hr) && allow_tearing;

		// スワップチェーン生成
		DXGI_SWAP_CHAIN_DESC1 swap_chain_desc1{};
		swap_chain_desc1.Width = _framebufferDimensions.cx;
		swap_chain_desc1.Height = _framebufferDimensions.cy;
		swap_chain_desc1.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swap_chain_desc1.Stereo = FALSE;
		swap_chain_desc1.SampleDesc.Count = 1;
		swap_chain_desc1.SampleDesc.Quality = 0;
		swap_chain_desc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap_chain_desc1.BufferCount = 2;
		swap_chain_desc1.Scaling = DXGI_SCALING_STRETCH;
		swap_chain_desc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swap_chain_desc1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swap_chain_desc1.Flags = _tearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
#ifdef X3DGP_FULLSCREEN
		hr = dxgiFactory6->CreateSwapChainForHwnd(device.Get(), hwnd, &swap_chain_desc1, NULL, NULL, swapChain.ReleaseAndGetAddressOf());
#endif
#if 0
		swap_chain_desc1.Flags |= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
#endif
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		hr = dxgiFactory6->MakeWindowAssociation(_hwnd, DXGI_MWA_NO_ALT_ENTER);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		Microsoft::WRL::ComPtr<ID3D11Texture2D> render_target_buffer;
		hr = _swapChain->GetBuffer(0, IID_PPV_ARGS(render_target_buffer.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		hr = _device->CreateRenderTargetView(render_target_buffer.Get(), NULL, _renderTargetView.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// 深度ステンシルビュー作成
	{
		// 参考資料
		// https://tositeru.github.io/ImasaraDX11//part/ZBuffer-and-depth-stencil
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer{};
		// 深度ステンシルバッファの作成
		D3D11_TEXTURE2D_DESC texture2dDesc{};
		texture2dDesc.Width = _framebufferDimensions.cx;
		texture2dDesc.Height = _framebufferDimensions.cy;
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
		hr = _device->CreateTexture2D(&texture2dDesc, NULL, depthStencilBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = texture2dDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		hr = _device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc,
			_depthStencilView.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	}

	// ビューポート設定
	{
		D3D11_VIEWPORT viewport{};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = static_cast<float>(_framebufferDimensions.cx);
		viewport.Height = static_cast<float>(_framebufferDimensions.cy);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		_immediateContext->RSSetViewports(1, &viewport);
	}
}
