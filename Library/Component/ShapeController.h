#pragma once

#include "Component.h"

enum class ShapeType
{
	Box,
	Sphere,
	Capsule,

	ShapeTypeMax,
};

class ShapeController : public Component
{
public:
	ShapeController(ShapeType type = ShapeType::Box) : _type(type){}
	~ShapeController()override {}

	// –¼‘OŽæ“¾
	const char* GetName()const { return "ShapeController"; }

	// •`‰æˆ—
	void Render(const RenderContext& rc) override;

	// ‰e•`‰æ
	void CastShadow(const RenderContext& rc) override;

	// GUI•`‰æ
	void DrawGui() override;

	ShapeType GetType()const { return _type; }
	const Vector4& GetColor()const { return _color; }

	void SetType(ShapeType type) { this->_type = type; }
	void SetColor(const Vector4& color) { this->_color = color; }
private:
	ShapeType _type;
	Vector4 _color = VECTOR4_WHITE;
	float _radius = 0.5f;
	float _height = 1.0f;
};