#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Input/DebugInput.h"
#include "Camera/DebugCamera.h"
#include "Renderer/DebugRenderer.h"

namespace Debug
{
	/// <summary>
	/// 指定のボタンが有効化かどうか
	/// </summary>
	/// <param name="button">どのボタンか</param>
	/// <returns>有効ならtrue</returns>
	bool IsActive(DebugButton button);

	/// <summary>
	/// デバッグ文字出力
	/// </summary>
	/// <param name="str"></param>
	void OutputString(std::string str);
	/// <summary>
	/// デバッグ文字出力
	/// </summary>
	/// <param name="str"></param>
	void OutputString(std::wstring str);

	/// <summary>
	/// 箱描画
	/// </summary>
	/// <param name="position"></param>
	/// <param name="angle"></param>
	/// <param name="size"></param>
	/// <param name="color"></param>
	void DrawBox(
		const Vector3& position,
		const Vector3& angle,
		const Vector3& size,
		const Vector4& color = { 1,1,1,1 });

	/// <summary>
	/// 箱描画
	/// </summary>
	/// <param name="transform"></param>
	/// <param name="color"></param>
	void DrawBox(
		const DirectX::XMFLOAT4X4& transform,
		const Vector4& color);

	/// <summary>
	/// 球描画
	/// </summary>
	/// <param name="position"></param>
	/// <param name="radius"></param>
	/// <param name="color"></param>
	void DrawSphere(
		const Vector3& position,
		float radius,
		const Vector4& color = { 1,1,1,1 });

	/// <summary>
	/// カプセル描画
	/// </summary>
	/// <param name="transform"></param>
	/// <param name="radius"></param>
	/// <param name="height"></param>
	/// <param name="color"></param>
	void DrawCapsule(
		const DirectX::XMFLOAT4X4& transform,
		float radius,
		float height,
		const Vector4& color = { 1,1,1,1 });
	/// <summary>
	/// カプセル描画
	/// </summary>
	/// <param name="start"></param>
	/// <param name="end"></param>
	/// <param name="radius"></param>
	/// <param name="color"></param>
	void DrawCapsule(
		const Vector3& start,
		const Vector3& end,
		float radius,
		const Vector4& color = { 1,1,1,1 });

	/// <summary>
	/// 骨描画
	/// </summary>
	/// <param name="transform"></param>
	/// <param name="length"></param>
	/// <param name="color"></param>
	void DrawBone(
		const DirectX::XMFLOAT4X4& transform,
		float length,
		const Vector4& color = { 1,1,1,1 });

	/// <summary>
	/// 矢印描画
	/// </summary>
	/// <param name="start"></param>
	/// <param name="target"></param>
	/// <param name="radius"></param>
	/// <param name="color"></param>
	void DrawArrow(
		const Vector3& start,
		const Vector3& target,
		float radius,
		const Vector4& color = { 1,1,1,1 });

	/// <summary>
	/// 軸描画
	/// </summary>
	/// <param name="transform"></param>
	void DrawAxis(const DirectX::XMFLOAT4X4& transform);

	/// <summary>
	/// グリッド描画
	/// </summary>
	/// <param name="subdivisions">分割数</param>
	/// <param name="scale">1マスの大きさ</param>
	void DrawGrid(int subdivisions, float scale = 1.0f);
	
	/// <summary>
	/// 頂点追加
	/// </summary>
	/// <param name="position"></param>
	void AddVertex(const Vector3& position);

	/// <summary>
	/// 描画実行
	/// </summary>
	void Render(
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection);

	/// <summary>
	/// デバッグカメラ取得
	/// </summary>
	/// <returns></returns>
	DebugCamera* GetDebugCamera();
}

class DebugSupporter
{
private:
	DebugSupporter() {}
	~DebugSupporter() {}

public:
	// インスタンス取得
	static DebugSupporter& Instance() {
		static DebugSupporter ins_;
		return ins_;
	}

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(float elapsedTime);

	/// <summary>
	/// デバッグGUI表示
	/// </summary>
	void DrawGui();

	/// <summary>
	/// stringを出力に表示
	/// </summary>
	/// <returns></returns>
	void OutputString(std::string str);
	void OutputString(std::wstring str);

	/// <summary>
	/// デバッグ入力取得
	/// </summary>
	/// <returns></returns>
	DebugInput* GetDebugInput() { return &debugInput_; }

	/// <summary>
	/// デバッグカメラ取得
	/// </summary>
	/// <returns></returns>
	DebugCamera* GetDebugCamera() { return &debugCamera_; }

	/// <summary>
	/// デバッグ描画オブジェクト取得
	/// </summary>
	/// <returns></returns>
	DebugRenderer* GetDebugRenderer() { return debugRenderer_.get(); }
private:
	std::vector<std::string> debugStrings_;
	DebugInput debugInput_;
	DebugCamera debugCamera_;
	std::unique_ptr<DebugRenderer> debugRenderer_;

	bool showCameraGui_ = false;
};