#pragma once
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <algorithm>

#include "../../Library/Math/RectTransform.h"
#include "../../Library/2D/Sprite.h"
#include "../../Library/Scene/Scene.h"

// UIウィジェット基底クラス
class UIWidget : public std::enable_shared_from_this<UIWidget> 
{
public:
	UIWidget() = default;
    virtual ~UIWidget() = default;

    // 子要素を追加するメソッド
	template <typename T>
	std::shared_ptr<T> AddChild(std::shared_ptr<T> child)
	{
		child->_parent = weak_from_this();
		_children.push_back(child);
		return child;
	}
	// 子要素を追加するメソッド
	std::shared_ptr<UIWidget> AddChild(std::shared_ptr<UIWidget> child)
	{
		return AddChild<UIWidget>(child);
	}
	// 更新
	void Update(float elapsedTime);
	// 描画
	void Render(Scene* scene);
	// トランスフォーム更新
	void UpdateTransform(const RectTransform* parent);

#pragma region 共通インターフェース
	// 更新のオーバーライド用
	virtual void OnUpdate(float elapsedTime) {}
	// 描画のオーバーライド用
	virtual void OnRender(Scene* scene) {}
	// 選択された時
	virtual void OnSelected() {}
	// 起動時
	virtual void OnActivated();
	// 停止時
	virtual void OnDeactivated();
	// フォーカスされた時
	virtual void OnFocused();
	// フォーカスが外れた時
	virtual void OnUnfocused();
#pragma endregion

#pragma region アクセサ
	const std::string& GetName() { return _name; }
	void SetName(const std::string& name) { _name = name; }
	std::shared_ptr<UIWidget> GetParent() const { return _parent.lock(); }
	const std::vector<std::shared_ptr<UIWidget>>& GetChildren() const { return _children; }

	bool IsActive() const { return _isActive; }
	void SetActive(bool active);

	bool IsFocused() const { return _isFocused; }
	void SetFocus(bool focus);

	RectTransform& GetRectTransform() { return _rectTransform; }
#pragma endregion

#pragma region デバッグ
	// GUI描画
	// GUIで選択した子供のウィジェットを返す場合がある
	std::shared_ptr<UIWidget> DrawGui();
	// GUI描画のオーバーライド用
	virtual void OnDrawGui() {}
#pragma endregion

protected:
	std::string _name{};
    std::weak_ptr<UIWidget> _parent;
    std::vector<std::shared_ptr<UIWidget>> _children;
	RectTransform _rectTransform;

	// 起動しているか
	bool _isActive = false;
    // フォーカス（選択）されているか
    bool _isFocused = false;
};

// 画像ウィジェット
class UIImage : public UIWidget
{
public:
	// 描画
	void OnRender(Scene* scene) override;
	// GUI描画
	void OnDrawGui() override;

	// 画像読み込み
	void LoadTexture(const wchar_t* filename, 
		Sprite::CenterAlignment alignment = Sprite::CenterAlignment::CenterCenter);

protected:
	Sprite _sprite;
};

// 文字列ウィジェット
class UIText : public UIWidget
{
public:
	// 更新
	void OnUpdate(float elapsedTime) override;
	// 描画
	void OnRender(Scene* scene) override;
	// GUI描画
	void OnDrawGui() override;

	// 文字列設定
	void SetText(const std::wstring& text) { _textData.text = text; }

protected:
	TextRenderer::TextDrawData _textData;

	// フォーカスされていない時の色
	Vector4 _normalColor = Vector4::White;
	// フォーカスされている時の色
	Vector4 _focusColor = Vector4::Red;
};

// 垂直ボックスレイアウトウィジェット
class UIVerticalBox : public UIWidget
{
public:
	// 更新
	void OnUpdate(float elapsedTime) override;
	// GUI描画
	void OnDrawGui() override;
	// 起動時
	void OnActivated() override;

protected:
	// 選択中の子ウィジェットインデックス
	int _selectedIndex = 0;

	float _height = 50.0f;
	float _padding = 10.0f;
};

// リストビューウィジェット
template <typename T>
class UIListView : public UIVerticalBox
{
	std::vector<T> _dataList;
	// データ1個につき、どういうWidgetを生成するかを決める関数
	std::function<std::shared_ptr<UIWidget>(const T&)> _rowGenerator;

public:
	void SetModel(const std::vector<T>& data,
		std::function<std::shared_ptr<UIWidget>(const T&)> generator)
	{
		_dataList = data;
		_rowGenerator = generator;
		Rebuild();
	}

	void Rebuild()
	{
		_children.clear();
		for (const auto& item : _dataList)
		{
			// データからWidget（行）を生成して追加
			auto rowWidget = _rowGenerator(item);
			AddChild(rowWidget);
		}
	}
};

// ボタンウィジェット
// 押された時にコールバックを呼び出す
class UIButton : public UIWidget
{
public:
	// 更新
	void OnUpdate(float elapsedTime) override;
	// 描画
	void OnRender(Scene* scene) override;
	// GUI描画
	void OnDrawGui() override;
	// クリック時のコールバック設定
	void SetOnClick(std::function<void()> callback) { _onClick = callback; }
protected:
	std::function<void()> _onClick = nullptr;
};