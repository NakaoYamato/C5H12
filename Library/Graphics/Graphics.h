#pragma once

#include <d3d11_1.h>
#include <dxgi1_6.h>
#include <memory>
#include <wrl.h>
#include <vector>
#include <mutex>

#include "RenderState.h"
#include "ConstantBufferManager.h"
#include "../PostProcess/FrameBuffer.h"
#include "../PostProcess/CascadedShadowMap/CascadedShadowMap.h"
#include "GBuffer.h"

#define X3DGP_FULLSCREEN

class Graphics
{
private:
	Graphics() {}
	Graphics(const Graphics&) = delete;
	~Graphics() {}

public:
	// インスタンス取得
	static Graphics& Instance() {
		static Graphics ins;
		return ins;
	}

	// 初期化処理
	void Initialize(HWND hwnd,
		HINSTANCE instance,
		CONST BOOL FULLSCREEN);

	// フルスクリーンの切り替え
	void StylizeWindow(BOOL fullscreen);

	// 画面サイズ変更
	void OnSizeChanged(UINT width, UINT height);

	// バックバッファに描画した画を画面に表示する。
	void Present(UINT syncInterval);

	// デバイス取得
	// [[nodiscard]] を付けた関数は戻り値を無視すると警告を出す
	[[nodiscard]] ID3D11Device* GetDevice() const { return _device.Get(); }

	// デバイスコンテキスト取得
	[[nodiscard]] ID3D11DeviceContext* GetDeviceContext() const { return _immediateContext.Get(); }

	// スワップチェーン取得
	[[nodiscard]] IDXGISwapChain* GetSwapChain() const { return _swapChain.Get(); }

	// レンダーターゲットビュー取得
	[[nodiscard]] ID3D11RenderTargetView* GetRenderTargetView() const { return _renderTargetView.Get(); }
	[[nodiscard]] ID3D11RenderTargetView* const* GetAddressOfRenderTargetView() const { return _renderTargetView.GetAddressOf(); }

	// デプスステンシルビュー取得
	[[nodiscard]] ID3D11DepthStencilView* GetDepthStencilView() const { return _depthStencilView.Get(); }

	// レンダーステート取得
	[[nodiscard]] RenderState* GetRenderState() { return _renderState.get(); }

	// ミューテックス取得
	[[nodiscard]] std::mutex& GetMutex() { return _mutex; }

	// メインウィンドウのハンドル取得
	[[nodiscard]] HWND& GetHwnd() { return _hwnd; }

	// メインウィンドウのインスタンス取得
	[[nodiscard]] HINSTANCE& GetHInstance() { return _hInstance; }

	// フルスクリーンかどうか
	[[nodiscard]] const BOOL& GetFullscreenMode() const { return _fullscreenMode; }

	// スクリーン幅取得
	[[nodiscard]] float GetScreenWidth() const { return static_cast<float>(_framebufferDimensions.cx); }

	// スクリーン高さ取得
	[[nodiscard]] float GetScreenHeight() const { return static_cast<float>(_framebufferDimensions.cy); }

	// 定数バッファの管理者取得
	[[nodiscard]] ConstantBufferManager* GetConstantBufferManager() { return _constantBufferManager.get(); }

	// フレームバッファ取得
	[[nodiscard]] FrameBuffer* GetFrameBuffer(int index) { return _frameBufferes[index].get(); }

	// カスケードシャドウマップ取得
	[[nodiscard]] CascadedShadowMap* GetCascadedShadowMap() { return _cascadedShadowMap.get(); }

	// マルチレンダーターゲット取得
	[[nodiscard]] GBuffer* GetGBuffer() { return _gBuffer.get(); }

	// デファードレンダリングかどうか
	[[nodiscard]] int RenderingDeferred() const { return _renderingDeferred; }

	// ImGui描画
	void DrawGui();
private:
	// フルスクリーン用（福井先生の秘密フォルダx3dgp.fullscreen参照）
	// 高性能アダプターの取得
	void AcquireHighPerformanceAdapter(IDXGIFactory6* dxgiFactory6, IDXGIAdapter3** dxgiAdapter3);

	// スワップチェーンの作成
	void CreateSwapChain(IDXGIFactory6* dxgi_factory6);

private:
	// COMオブジェクトポインタ
	Microsoft::WRL::ComPtr<ID3D11Device>			_device = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		_immediateContext = nullptr;
#ifdef X3DGP_FULLSCREEN
	Microsoft::WRL::ComPtr<IDXGISwapChain1>			_swapChain = nullptr;
#else
	Microsoft::WRL::ComPtr<IDXGISwapChain>			_swapChain = nullptr;
#endif
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	_renderTargetView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	_depthStencilView = nullptr;

	// 各種ステートの管理者
	std::unique_ptr<RenderState>					_renderState;
	// 定数バッファの管理者
	std::unique_ptr<ConstantBufferManager> _constantBufferManager;
	// オフスクリーンレンダリングの管理者
	std::unique_ptr<FrameBuffer> _frameBufferes[4];
	// カスケードシャドウマップ
	std::unique_ptr<CascadedShadowMap> _cascadedShadowMap;
	// マルチレンダーターゲットの管理者
	std::unique_ptr<GBuffer> _gBuffer;

	// デバッグ用
	bool _renderingDeferred = true;
	bool _drawFrameBuffer = false;
	bool _drawCSMGui = false;
	bool _drawGBGui = false;
private:
	// スクリーンの大きさ
	SIZE _framebufferDimensions{};

	// DeviceContextを同時アクセスさせないための排他制御用オブジェクト
	std::mutex _mutex;

	// メインウィンドウのハンドル
	HWND _hwnd{};
	// メインウィンドウのインスタンス
	HINSTANCE _hInstance = {};

	// フルスクリーンのフラグ
	BOOL _fullscreenMode = FALSE;
	BOOL _tearingSupported = FALSE;

	RECT _oldWindowRect{};
	DWORD _windowedStyle{};
	Microsoft::WRL::ComPtr<IDXGIAdapter3> _adapter = nullptr;
};