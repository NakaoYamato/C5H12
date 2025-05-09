#pragma once

#include "ModelRenderer.h"

enum class ShapeType
{
	Box,
	Sphere,
	Capsule,

	ShapeTypeMax,
};

class ShapeController : public ModelRenderer
{
public:
	ShapeController(ShapeType type = ShapeType::Box) :
		_type(type) {}
	~ShapeController()override {}

	// 名前取得
	const char* GetName()const { return "ShapeController"; }

	// 初期化
	void Start()override;

	// GUI描画
	void DrawGui() override;

#pragma region アクセサ
	ShapeType GetType()const { return _type; }

	void SetType(ShapeType type) { this->_type = type; }
#pragma endregion
private:
	// タイプに合わせたモデルのファイルパス取得
	const char* GetShapeModelFilename(ShapeType type) const;

private:
	ShapeType _type;
	float _radius = 0.5f;
	float _height = 1.0f;
};