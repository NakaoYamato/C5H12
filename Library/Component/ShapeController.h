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
	ShapeController(ShapeType type = ShapeType::Box) : type_(type){}
	~ShapeController()override {}

	// –¼‘OŽæ“¾
	const char* GetName()const { return "ShapeController"; }

	// •`‰æˆ—
	void Render(const RenderContext& rc) override;

	// GUI•`‰æ
	void DrawGui() override;

	ShapeType GetType()const { return type_; }
	const Vector4& GetColor()const { return color_; }

	void SetType(ShapeType type) { this->type_ = type; }
	void SetColor(const Vector4& color) { this->color_ = color; }
private:
	ShapeType type_;
	Vector4 color_ = VECTOR4_WHITE;
	float radius_ = 0.5f;
	float height_ = 1.0f;
};