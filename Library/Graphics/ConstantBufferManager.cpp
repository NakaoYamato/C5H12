#include "ConstantBufferManager.h"
#include "../ResourceManager/GpuResourceManager.h"

ConstantBufferManager::ConstantBufferManager(ID3D11Device* device)
{
	// シーン用定数バッファ
	(void)GpuResourceManager::CreateConstantBuffer(
		device,
		sizeof(SceneConstantBuffer),
		constantBuffers_[static_cast<size_t>(ConstantBufferType::SceneCB)].GetAddressOf());
	// ライト用定数バッファ
	(void)GpuResourceManager::CreateConstantBuffer(
		device,
		sizeof(LightConstantBuffer),
		constantBuffers_[static_cast<size_t>(ConstantBufferType::LightCB)].GetAddressOf());
}

// 定数バッファの一括更新
void ConstantBufferManager::Update(RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	// シーン定数バッファの更新
	{
		ConstantBufferManager::SceneConstantBuffer sceneCB{};
		DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.camera->view_);
		DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.camera->projection_);
		DirectX::XMStoreFloat4x4(&sceneCB.viewProjection, V * P);
		sceneCB.lightDirection = rc.lightDirection;
		sceneCB.lightColor = rc.lightColor;
		const DirectX::XMFLOAT3& eye = rc.camera->eye_;
		sceneCB.cameraPosition.x = eye.x;
		sceneCB.cameraPosition.y = eye.y;
		sceneCB.cameraPosition.z = eye.z;
		DirectX::XMStoreFloat4x4(&sceneCB.invProjection, DirectX::XMMatrixInverse(nullptr, P));
		DirectX::XMStoreFloat4x4(&sceneCB.invView, DirectX::XMMatrixInverse(nullptr, V));
		DirectX::XMStoreFloat4x4(&sceneCB.invViewProjection, DirectX::XMMatrixInverse(nullptr, V * P));

		UpdateSceneCB(&sceneCB);
		dc->UpdateSubresource(constantBuffers_[static_cast<size_t>(ConstantBufferType::SceneCB)].Get(), 0, 0, &this->sceneCB_, 0, 0);
	}
	// ライト定数バッファの更新
	{
		ConstantBufferManager::LightConstantBuffer lightCB{};
		lightCB.ambientColor = rc.lightAmbientColor;
		lightCB.directionalLightDirection = rc.lightDirection;
		lightCB.directionalLightColor = rc.lightColor;
		size_t pointLightCount = rc.pointLights.size();
		if (pointLightCount > 8)
			pointLightCount = 8;
		for (size_t i = 0; i < pointLightCount; ++i)
		{
			lightCB.pointLights[i] = *rc.pointLights[i];
		}

		UpdateLightCB(&lightCB);
		dc->UpdateSubresource(constantBuffers_[static_cast<size_t>(ConstantBufferType::LightCB)].Get(), 0, 0, &this->lightCB_, 0, 0);
	}
}

/// 指定の定数バッファの更新
void ConstantBufferManager::UpdateCB(ID3D11DeviceContext* dc,
	ConstantBufferType bufferType,
	const std::unordered_map<std::string, float>& parameter)
{
	switch (bufferType)
	{
	case ConstantBufferType::SceneCB:
		UpdateSceneCB(parameter);
		dc->UpdateSubresource(constantBuffers_[static_cast<size_t>(ConstantBufferType::SceneCB)].Get(), 0, 0, &sceneCB_, 0, 0);
		break;
	case ConstantBufferType::LightCB:
		UpdateLightCB(parameter);
		dc->UpdateSubresource(constantBuffers_[static_cast<size_t>(ConstantBufferType::LightCB)].Get(), 0, 0, &lightCB_, 0, 0);
		break;
	default:
		assert(!"bufferType overflow");
		break;
	}
}

/// 指定の定数バッファの更新
void ConstantBufferManager::UpdateCB(ID3D11DeviceContext* dc,
	ConstantBufferType bufferType, 
	ConstantBufferBase* parameter)
{
	switch (bufferType)
	{
	case ConstantBufferType::SceneCB:
		UpdateSceneCB((SceneConstantBuffer*)(parameter));
		dc->UpdateSubresource(constantBuffers_[static_cast<size_t>(ConstantBufferType::SceneCB)].Get(), 0, 0, &sceneCB_, 0, 0);
		break;
	case ConstantBufferType::LightCB:
		UpdateLightCB((LightConstantBuffer*)(parameter));
		dc->UpdateSubresource(constantBuffers_[static_cast<size_t>(ConstantBufferType::LightCB)].Get(), 0, 0, &sceneCB_, 0, 0);
		break;
	default:
		assert(!"bufferType overflow");
		break;
	}
}

/// 指定の定数バッファをセット
void ConstantBufferManager::SetCB(ID3D11DeviceContext* dc,
	UINT slot, 
	ConstantBufferType bufferType,
	ConstantUpdateTarget updateTarget)
{
	const size_t bufferIndex = static_cast<size_t>(bufferType);
#ifdef _DEBUG
	if(bufferIndex >= static_cast<size_t>(ConstantBufferType::CBtypeMax))
		assert(!"bufferType overflow");

#endif

	int bit = static_cast<int>(updateTarget);
	auto SetFromTarget = [&](ID3D11Buffer* const* buffer)->void
		{
			if (bit & static_cast<int>(ConstantUpdateTarget::VERTEX))
				dc->VSSetConstantBuffers(slot, 1, buffer);
			if (bit & static_cast<int>(ConstantUpdateTarget::GEOMETORY))
				dc->GSSetConstantBuffers(slot, 1, buffer);
			if (bit & static_cast<int>(ConstantUpdateTarget::PIXEL))
				dc->PSSetConstantBuffers(slot, 1, buffer);
			if (bit & static_cast<int>(ConstantUpdateTarget::COMPUTE))
				dc->CSSetConstantBuffers(slot, 1, buffer);
		};
	SetFromTarget(constantBuffers_[bufferIndex].GetAddressOf());
}

// シーン定数バッファの更新
void ConstantBufferManager::UpdateSceneCB(const std::unordered_map<std::string, float>& parameter)
{
	auto iter = parameter.begin();
	auto SetParameter = [&](float& target, const std::string& variable)->void
		{
			iter = parameter.find(variable.c_str());
			if (iter != parameter.end())
				target = (*iter).second;
		};

	SetParameter(sceneCB_.viewProjection._11, "viewProjection._11");
	SetParameter(sceneCB_.viewProjection._12, "viewProjection._12");
	SetParameter(sceneCB_.viewProjection._13, "viewProjection._13");
	SetParameter(sceneCB_.viewProjection._14, "viewProjection._14");
	SetParameter(sceneCB_.viewProjection._21, "viewProjection._21");
	SetParameter(sceneCB_.viewProjection._22, "viewProjection._22");
	SetParameter(sceneCB_.viewProjection._23, "viewProjection._23");
	SetParameter(sceneCB_.viewProjection._24, "viewProjection._24");
	SetParameter(sceneCB_.viewProjection._31, "viewProjection._31");
	SetParameter(sceneCB_.viewProjection._32, "viewProjection._32");
	SetParameter(sceneCB_.viewProjection._33, "viewProjection._33");
	SetParameter(sceneCB_.viewProjection._34, "viewProjection._34");
	SetParameter(sceneCB_.viewProjection._41, "viewProjection._41");
	SetParameter(sceneCB_.viewProjection._42, "viewProjection._42");
	SetParameter(sceneCB_.viewProjection._43, "viewProjection._43");
	SetParameter(sceneCB_.viewProjection._44, "viewProjection._44");

	SetParameter(sceneCB_.lightDirection.x, "lightDirection.x");
	SetParameter(sceneCB_.lightDirection.y, "lightDirection.y");
	SetParameter(sceneCB_.lightDirection.z, "lightDirection.z");
	SetParameter(sceneCB_.lightDirection.w, "lightDirection.w");

	SetParameter(sceneCB_.lightColor.x, "lightColor.x");
	SetParameter(sceneCB_.lightColor.y, "lightColor.y");
	SetParameter(sceneCB_.lightColor.z, "lightColor.z");
	SetParameter(sceneCB_.lightColor.w, "lightColor.w");

	SetParameter(sceneCB_.cameraPosition.x, "cameraPosition.x");
	SetParameter(sceneCB_.cameraPosition.y, "cameraPosition.y");
	SetParameter(sceneCB_.cameraPosition.z, "cameraPosition.z");
	SetParameter(sceneCB_.cameraPosition.w, "cameraPosition.w");

	SetParameter(sceneCB_.invView._11, "invView._11");
	SetParameter(sceneCB_.invView._12, "invView._12");
	SetParameter(sceneCB_.invView._13, "invView._13");
	SetParameter(sceneCB_.invView._14, "invView._14");
	SetParameter(sceneCB_.invView._21, "invView._21");
	SetParameter(sceneCB_.invView._22, "invView._22");
	SetParameter(sceneCB_.invView._23, "invView._23");
	SetParameter(sceneCB_.invView._24, "invView._24");
	SetParameter(sceneCB_.invView._31, "invView._31");
	SetParameter(sceneCB_.invView._32, "invView._32");
	SetParameter(sceneCB_.invView._33, "invView._33");
	SetParameter(sceneCB_.invView._34, "invView._34");
	SetParameter(sceneCB_.invView._41, "invView._41");
	SetParameter(sceneCB_.invView._42, "invView._42");
	SetParameter(sceneCB_.invView._43, "invView._43");
	SetParameter(sceneCB_.invView._44, "invView._44");

	SetParameter(sceneCB_.invProjection._11, "invProjection._11");
	SetParameter(sceneCB_.invProjection._12, "invProjection._12");
	SetParameter(sceneCB_.invProjection._13, "invProjection._13");
	SetParameter(sceneCB_.invProjection._14, "invProjection._14");
	SetParameter(sceneCB_.invProjection._21, "invProjection._21");
	SetParameter(sceneCB_.invProjection._22, "invProjection._22");
	SetParameter(sceneCB_.invProjection._23, "invProjection._23");
	SetParameter(sceneCB_.invProjection._24, "invProjection._24");
	SetParameter(sceneCB_.invProjection._31, "invProjection._31");
	SetParameter(sceneCB_.invProjection._32, "invProjection._32");
	SetParameter(sceneCB_.invProjection._33, "invProjection._33");
	SetParameter(sceneCB_.invProjection._34, "invProjection._34");
	SetParameter(sceneCB_.invProjection._41, "invProjection._41");
	SetParameter(sceneCB_.invProjection._42, "invProjection._42");
	SetParameter(sceneCB_.invProjection._43, "invProjection._43");
	SetParameter(sceneCB_.invProjection._44, "invProjection._44");

	SetParameter(sceneCB_.invViewProjection._11, "invViewProjection._11");
	SetParameter(sceneCB_.invViewProjection._12, "invViewProjection._12");
	SetParameter(sceneCB_.invViewProjection._13, "invViewProjection._13");
	SetParameter(sceneCB_.invViewProjection._14, "invViewProjection._14");
	SetParameter(sceneCB_.invViewProjection._21, "invViewProjection._21");
	SetParameter(sceneCB_.invViewProjection._22, "invViewProjection._22");
	SetParameter(sceneCB_.invViewProjection._23, "invViewProjection._23");
	SetParameter(sceneCB_.invViewProjection._24, "invViewProjection._24");
	SetParameter(sceneCB_.invViewProjection._31, "invViewProjection._31");
	SetParameter(sceneCB_.invViewProjection._32, "invViewProjection._32");
	SetParameter(sceneCB_.invViewProjection._33, "invViewProjection._33");
	SetParameter(sceneCB_.invViewProjection._34, "invViewProjection._34");
	SetParameter(sceneCB_.invViewProjection._41, "invViewProjection._41");
	SetParameter(sceneCB_.invViewProjection._42, "invViewProjection._42");
	SetParameter(sceneCB_.invViewProjection._43, "invViewProjection._43");
	SetParameter(sceneCB_.invViewProjection._44, "invViewProjection._44");
}

// シーン定数バッファの更新
void ConstantBufferManager::UpdateSceneCB(SceneConstantBuffer* buffer)
{
	if (buffer != nullptr)
	{
		this->sceneCB_.viewProjection = buffer->viewProjection;
		this->sceneCB_.lightDirection = buffer->lightDirection;
		this->sceneCB_.lightColor = buffer->lightColor;
		this->sceneCB_.cameraPosition = buffer->cameraPosition;
		this->sceneCB_.invProjection = buffer->invProjection;
		this->sceneCB_.invView = buffer->invView;
		this->sceneCB_.invViewProjection = buffer->invViewProjection;
	}
}

// ライト定数バッファの更新
void ConstantBufferManager::UpdateLightCB(const std::unordered_map<std::string, float>& parameter)
{
	auto iter = parameter.begin();
	auto SetParameter = [&](float& target, const std::string& variable)->void
		{
			iter = parameter.find(variable.c_str());
			if (iter != parameter.end())
				target = (*iter).second;
		};

	SetParameter(lightCB_.ambientColor.x, "ambientColor.x");
	SetParameter(lightCB_.ambientColor.y, "ambientColor.y");
	SetParameter(lightCB_.ambientColor.z, "ambientColor.z");
	SetParameter(lightCB_.ambientColor.w, "ambientColor.w");

	SetParameter(lightCB_.directionalLightDirection.x, "directionalLightDirection.x");
	SetParameter(lightCB_.directionalLightDirection.y, "directionalLightDirection.y");
	SetParameter(lightCB_.directionalLightDirection.z, "directionalLightDirection.z");
	SetParameter(lightCB_.directionalLightDirection.w, "directionalLightDirection.w");

	SetParameter(lightCB_.directionalLightColor.x, "directionalLightColor.x");
	SetParameter(lightCB_.directionalLightColor.y, "directionalLightColor.y");
	SetParameter(lightCB_.directionalLightColor.z, "directionalLightColor.z");
	SetParameter(lightCB_.directionalLightColor.w, "directionalLightColor.w");

	for (int i = 0; i < _countof(lightCB_.pointLights); i++)
	{
		std::string frontStr = "pointLights[";
		frontStr += std::to_string(i);
		frontStr += "].";
		SetParameter(lightCB_.pointLights[i].position.x, frontStr + "position.x");
		SetParameter(lightCB_.pointLights[i].position.y, frontStr + "position.y");
		SetParameter(lightCB_.pointLights[i].position.z, frontStr + "position.z");
		SetParameter(lightCB_.pointLights[i].position.w, frontStr + "position.w");

		SetParameter(lightCB_.pointLights[i].color.x, frontStr + "color.x");
		SetParameter(lightCB_.pointLights[i].color.y, frontStr + "color.y");
		SetParameter(lightCB_.pointLights[i].color.z, frontStr + "color.z");
		SetParameter(lightCB_.pointLights[i].color.w, frontStr + "color.w");

		SetParameter(lightCB_.pointLights[i].range, frontStr + "range");
		{
			std::string str = frontStr + "isAlive";
			iter = parameter.find(str.c_str());
			if (iter != parameter.end())
				lightCB_.pointLights[i].isAlive = (int)(*iter).second;
		}
	}
}

// ライト定数バッファの更新
void ConstantBufferManager::UpdateLightCB(LightConstantBuffer* buffer)
{
	if (buffer != nullptr)
	{
		this->lightCB_.ambientColor = buffer->ambientColor;
		this->lightCB_.directionalLightDirection = buffer->directionalLightDirection;
		this->lightCB_.directionalLightColor = buffer->directionalLightColor;
		for (int i = 0; i < _countof(lightCB_.pointLights); ++i)
		{
			this->lightCB_.pointLights[i] = buffer->pointLights[i];
		}
	}
}
