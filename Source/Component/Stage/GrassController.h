#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Component/ModelRenderer.h"

class GrassController : public Component
{
public:
    GrassController() {};
    ~GrassController()override {}

	// 名前取得
	const char* GetName()const { return "GrassController"; }

	// 開始処理
	void Start()override;

	// 更新処理
	void Update(float elapsedTime) override;

private:
	std::weak_ptr<ModelRenderer> _mr;
};

