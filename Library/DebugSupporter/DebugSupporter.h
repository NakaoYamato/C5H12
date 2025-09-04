#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "Input/DebugInput.h"
#include "Camera/DebugCamera.h"
#include "Renderer/DebugRenderer.h"

namespace Debug
{
	namespace Input
	{
		/// <summary>
		/// 指定のボタンが有効化かどうか
		/// </summary>
		/// <param name="button">どのボタンか</param>
		/// <returns>有効ならtrue</returns>
		bool IsActive(DebugButton button);
	}

	namespace Output
	{
		/// <summary>
		/// デバッグ文字出力
		/// </summary>
		/// <param name="str"></param>
		void String(std::string str);
		/// <summary>
		/// デバッグ文字出力
		/// </summary>
		/// <param name="str"></param>
		void String(std::wstring str);
		/// <summary>
		/// デバッグ文字出力
		/// </summary>
		/// <param name="value"></param>
		void String(int value);
		/// <summary>
		/// デバッグ文字出力
		/// </summary>
		/// <param name="value"></param>
		void String(float value);
		/// <summary>
		/// デバッグ文字出力
		/// </summary>
		/// <param name="value"></param>
		void String(const Vector3& value);
	}

	namespace Renderer
	{
		/// <summary>
		/// 箱(半径1m)描画
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
		/// 箱(半径1m)描画
		/// </summary>
		/// <param name="transform"></param>
		/// <param name="color"></param>
		void DrawBox(
			const DirectX::XMFLOAT4X4& transform,
			const Vector4& color);

		/// <summary>
		/// 箱(半径0.5m)描画
		/// </summary>
		/// <param name="position"></param>
		/// <param name="angle"></param>
		/// <param name="size"></param>
		/// <param name="color"></param>
		void DrawHalfBox(
			const Vector3& position,
			const Vector3& angle,
			const Vector3& size,
			const Vector4& color = { 1,1,1,1 });

		/// <summary>
		/// 箱(半径0.5m)描画
		/// </summary>
		/// <param name="transform"></param>
		/// <param name="color"></param>
		void DrawHalfBox(
			const DirectX::XMFLOAT4X4& transform,
			const Vector4& color = { 1,1,1,1 });

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
		void AddVertex(const Vector3& position, const Vector4& color = Vector4::White);

		/// <summary>
		/// 描画実行
		/// </summary>
		void Render(
			const DirectX::XMFLOAT4X4& view,
			const DirectX::XMFLOAT4X4& projection);

	}

	namespace Dialog
	{
		// ダイアログリザルト
		enum class DialogResult
		{
			Yes,
			No,
			OK,
			Cancel
		};

		// [ファイルを開く]ダイアログボックスを表示
		DialogResult OpenFileName(
			std::string& filepath,
			std::string& currentDirectory,
			const char* filter = nullptr, 
			const char* title = nullptr,
			HWND hwnd = NULL,
			bool multi_select = false);

		// [ファイルを保存]ダイアログボックスを表示
		DialogResult SaveFileName(
			char* filepath, 
			int size, 
			const char* filter = nullptr, 
			const char* title = nullptr,
			const char* ext = nullptr,
			HWND hwnd = NULL);
		DialogResult SaveFileName(
			std::string* filepath,
			const char* filter = nullptr,
			const char* title = nullptr,
			const char* ext = nullptr,
			HWND hwnd = NULL);
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
	/// ギズモ使用
	/// </summary>
	/// <param name="transform"></param>
	bool Guizmo(
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection,
		DirectX::XMFLOAT4X4* transform,
		int guizmoOperation = -1,
		int guizmoMode = -1);

	/// <summary>
	/// デバッグGUI表示
	/// </summary>
	void DrawGui();

	/// <summary>
	/// デバッグ入力取得
	/// </summary>
	/// <returns></returns>
	DebugInput* GetDebugInput();

	/// <summary>
	/// デバッグカメラ取得
	/// </summary>
	/// <returns></returns>
	DebugCamera* GetDebugCamera();

	/// <summary>
	/// デバッグ描画オブジェクト取得
	/// </summary>
	/// <returns></returns>
	DebugRenderer* GetDebugRenderer();
}
