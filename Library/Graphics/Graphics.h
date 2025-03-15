#pragma once

#include <d3d11_1.h>
#include <dxgi1_6.h>
#include <memory>
#include <wrl.h>
#include <vector>
#include <mutex>

#include "RenderState.h"
#include "ConstantBufferManager.h"
#include "RenderingManager.h"

//#define X3DGP_FULLSCREEN

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
		CONST BOOL FULLSCREEN);

	// フルスクリーンの切り替え
	void StylizeWindow(BOOL fullscreen);

	// 画面サイズ変更
	void OnSizeChanged(UINT width, UINT height);

	// バックバッファに描画した画を画面に表示する。
	void Present(UINT syncInterval);

	// デバイス取得
	// [[nodiscard]] を付けた関数は戻り値を無視すると警告を出す
	[[nodiscard]] ID3D11Device* GetDevice() const { return device.Get(); }

	// デバイスコンテキスト取得
	[[nodiscard]] ID3D11DeviceContext* GetDeviceContext() const { return immediateContext.Get(); }

	// スワップチェーン取得
	[[nodiscard]] IDXGISwapChain* GetSwapChain() const { return swapChain.Get(); }

	// レンダーターゲットビュー取得
	[[nodiscard]] ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }
	[[nodiscard]] ID3D11RenderTargetView* const* GetAddressOfRenderTargetView() const { return renderTargetView.GetAddressOf(); }

	// デプスステンシルビュー取得
	[[nodiscard]] ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }

	// レンダーステート取得
	[[nodiscard]] RenderState* GetRenderState() { return renderState.get(); }

	// ミューテックス取得
	[[nodiscard]] std::mutex& GetMutex() { return mutex_; }

	// メインウィンドウのハンドル取得
	[[nodiscard]] HWND& GetHwnd() { return hwnd_; }

	// フルスクリーンかどうか
	[[nodiscard]] const BOOL& GetFullscreenMode() const { return fullscreenMode_; }

	// スクリーン幅取得
	[[nodiscard]] float GetScreenWidth() const { return static_cast<float>(framebufferDimensions_.cx); }

	// スクリーン高さ取得
	[[nodiscard]] float GetScreenHeight() const { return static_cast<float>(framebufferDimensions_.cy); }

	// 定数バッファの管理者取得
	[[nodiscard]] ConstantBufferManager* GetConstantBufferManager() { return constantBufferManager.get(); }
	// 描画管理者取得
	[[nodiscard]] RenderingManager* GetRenderingManager() { return _renderingManager.get(); }

private:
	// フルスクリーン用（福井先生の秘密フォルダx3dgp.fullscreen参照）
	// 高性能アダプターの取得
	void AcquireHighPerformanceAdapter(IDXGIFactory6* dxgiFactory6, IDXGIAdapter3** dxgiAdapter3);

	// スワップチェーンの作成
	void CreateSwapChain(IDXGIFactory6* dxgi_factory6);

private:
	// COMオブジェクトポインタ
	Microsoft::WRL::ComPtr<ID3D11Device>			device = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediateContext = nullptr;
#ifdef X3DGP_FULLSCREEN
	Microsoft::WRL::ComPtr<IDXGISwapChain1>			swapChain = nullptr;
#else
	Microsoft::WRL::ComPtr<IDXGISwapChain>			swapChain = nullptr;
#endif
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView = nullptr;

	// 各種ステートの管理者
	std::unique_ptr<RenderState>					renderState;
	// 定数バッファの管理者
	std::unique_ptr<ConstantBufferManager> constantBufferManager;
	// 描画管理者
	std::unique_ptr<RenderingManager> _renderingManager;

private:
	// スクリーンの大きさ
	SIZE framebufferDimensions_{};

	// DeviceContextを同時アクセスさせないための排他制御用オブジェクト
	std::mutex mutex_;

	// メインウィンドウのハンドル
	HWND hwnd_{};

	// フルスクリーンのフラグ
	BOOL fullscreenMode_ = FALSE;
	BOOL tearingSupported_ = FALSE;

	RECT oldWindowRect_{};
	DWORD windowedStyle_{};
	Microsoft::WRL::ComPtr<IDXGIAdapter3> adapter_ = nullptr;
};