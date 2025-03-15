#include "ConstantBufferManager.h"
#include "../ResourceManager/GpuResourceManager.h"

ConstantBufferManager::ConstantBufferManager(ID3D11Device* device)
{
	// シーン用定数バッファ
	(void)GpuResourceManager::CreateConstantBuffer(
		device,
		sizeof(SceneConstantBuffer),
		_constantBuffers[static_cast<size_t>(ConstantBufferType::SceneCB)].GetAddressOf());
	// ライト用定数バッファ
	(void)GpuResourceManager::CreateConstantBuffer(
		device,
		sizeof(LightConstantBuffer),
		_constantBuffers[static_cast<size_t>(ConstantBufferType::LightCB)].GetAddressOf());
}

// 定数バッファの一括更新
void ConstantBufferManager::Update(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	// シーン定数バッファの更新
	{
		ConstantBufferManager::SceneConstantBuffer sceneCB{};
		DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.camera->view);
		DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.camera->projection);
		DirectX::XMStoreFloat4x4(&sceneCB.viewProjection, V * P);
		sceneCB.lightDirection = rc.lightDirection;
		sceneCB.lightColor = rc.lightColor;
		const DirectX::XMFLOAT3& eye = rc.camera->eye;
		sceneCB.cameraPosition.x = eye.x;
		sceneCB.cameraPosition.y = eye.y;
		sceneCB.cameraPosition.z = eye.z;
		DirectX::XMStoreFloat4x4(&sceneCB.invProjection, DirectX::XMMatrixInverse(nullptr, P));
		DirectX::XMStoreFloat4x4(&sceneCB.invView, DirectX::XMMatrixInverse(nullptr, V));
		DirectX::XMStoreFloat4x4(&sceneCB.invViewProjection, DirectX::XMMatrixInverse(nullptr, V * P));

		UpdateSceneCB(&sceneCB);
		dc->UpdateSubresource(_constantBuffers[static_cast<size_t>(ConstantBufferType::SceneCB)].Get(), 0, 0, &this->_sceneCB, 0, 0);
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
		dc->UpdateSubresource(_constantBuffers[static_cast<size_t>(ConstantBufferType::LightCB)].Get(), 0, 0, &this->_lightCB, 0, 0);
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
		dc->UpdateSubresource(_constantBuffers[static_cast<size_t>(ConstantBufferType::SceneCB)].Get(), 0, 0, &_sceneCB, 0, 0);
		break;
	case ConstantBufferType::LightCB:
		UpdateLightCB(parameter);
		dc->UpdateSubresource(_constantBuffers[static_cast<size_t>(ConstantBufferType::LightCB)].Get(), 0, 0, &_lightCB, 0, 0);
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
		dc->UpdateSubresource(_constantBuffers[static_cast<size_t>(ConstantBufferType::SceneCB)].Get(), 0, 0, &_sceneCB, 0, 0);
		break;
	case ConstantBufferType::LightCB:
		UpdateLightCB((LightConstantBuffer*)(parameter));
		dc->UpdateSubresource(_constantBuffers[static_cast<size_t>(ConstantBufferType::LightCB)].Get(), 0, 0, &_sceneCB, 0, 0);
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
			if (bit & static_cast<int>(ConstantUpdateTarget::Vertex))
				dc->VSSetConstantBuffers(slot, 1, buffer);
			if (bit & static_cast<int>(ConstantUpdateTarget::Hull))
				dc->HSSetConstantBuffers(slot, 1, buffer);
			if (bit & static_cast<int>(ConstantUpdateTarget::Domain))
				dc->DSSetConstantBuffers(slot, 1, buffer);
			if (bit & static_cast<int>(ConstantUpdateTarget::Geometory))
				dc->GSSetConstantBuffers(slot, 1, buffer);
			if (bit & static_cast<int>(ConstantUpdateTarget::Pixel))
				dc->PSSetConstantBuffers(slot, 1, buffer);
			if (bit & static_cast<int>(ConstantUpdateTarget::Compute))
				dc->CSSetConstantBuffers(slot, 1, buffer);
		};
	SetFromTarget(_constantBuffers[bufferIndex].GetAddressOf());
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

	SetParameter(_sceneCB.viewProjection._11, "viewProjection._11");
	SetParameter(_sceneCB.viewProjection._12, "viewProjection._12");
	SetParameter(_sceneCB.viewProjection._13, "viewProjection._13");
	SetParameter(_sceneCB.viewProjection._14, "viewProjection._14");
	SetParameter(_sceneCB.viewProjection._21, "viewProjection._21");
	SetParameter(_sceneCB.viewProjection._22, "viewProjection._22");
	SetParameter(_sceneCB.viewProjection._23, "viewProjection._23");
	SetParameter(_sceneCB.viewProjection._24, "viewProjection._24");
	SetParameter(_sceneCB.viewProjection._31, "viewProjection._31");
	SetParameter(_sceneCB.viewProjection._32, "viewProjection._32");
	SetParameter(_sceneCB.viewProjection._33, "viewProjection._33");
	SetParameter(_sceneCB.viewProjection._34, "viewProjection._34");
	SetParameter(_sceneCB.viewProjection._41, "viewProjection._41");
	SetParameter(_sceneCB.viewProjection._42, "viewProjection._42");
	SetParameter(_sceneCB.viewProjection._43, "viewProjection._43");
	SetParameter(_sceneCB.viewProjection._44, "viewProjection._44");

	SetParameter(_sceneCB.lightDirection.x, "lightDirection.x");
	SetParameter(_sceneCB.lightDirection.y, "lightDirection.y");
	SetParameter(_sceneCB.lightDirection.z, "lightDirection.z");
	SetParameter(_sceneCB.lightDirection.w, "lightDirection.w");

	SetParameter(_sceneCB.lightColor.x, "lightColor.x");
	SetParameter(_sceneCB.lightColor.y, "lightColor.y");
	SetParameter(_sceneCB.lightColor.z, "lightColor.z");
	SetParameter(_sceneCB.lightColor.w, "lightColor.w");

	SetParameter(_sceneCB.cameraPosition.x, "cameraPosition.x");
	SetParameter(_sceneCB.cameraPosition.y, "cameraPosition.y");
	SetParameter(_sceneCB.cameraPosition.z, "cameraPosition.z");
	SetParameter(_sceneCB.cameraPosition.w, "cameraPosition.w");

	SetParameter(_sceneCB.invView._11, "invView._11");
	SetParameter(_sceneCB.invView._12, "invView._12");
	SetParameter(_sceneCB.invView._13, "invView._13");
	SetParameter(_sceneCB.invView._14, "invView._14");
	SetParameter(_sceneCB.invView._21, "invView._21");
	SetParameter(_sceneCB.invView._22, "invView._22");
	SetParameter(_sceneCB.invView._23, "invView._23");
	SetParameter(_sceneCB.invView._24, "invView._24");
	SetParameter(_sceneCB.invView._31, "invView._31");
	SetParameter(_sceneCB.invView._32, "invView._32");
	SetParameter(_sceneCB.invView._33, "invView._33");
	SetParameter(_sceneCB.invView._34, "invView._34");
	SetParameter(_sceneCB.invView._41, "invView._41");
	SetParameter(_sceneCB.invView._42, "invView._42");
	SetParameter(_sceneCB.invView._43, "invView._43");
	SetParameter(_sceneCB.invView._44, "invView._44");

	SetParameter(_sceneCB.invProjection._11, "invProjection._11");
	SetParameter(_sceneCB.invProjection._12, "invProjection._12");
	SetParameter(_sceneCB.invProjection._13, "invProjection._13");
	SetParameter(_sceneCB.invProjection._14, "invProjection._14");
	SetParameter(_sceneCB.invProjection._21, "invProjection._21");
	SetParameter(_sceneCB.invProjection._22, "invProjection._22");
	SetParameter(_sceneCB.invProjection._23, "invProjection._23");
	SetParameter(_sceneCB.invProjection._24, "invProjection._24");
	SetParameter(_sceneCB.invProjection._31, "invProjection._31");
	SetParameter(_sceneCB.invProjection._32, "invProjection._32");
	SetParameter(_sceneCB.invProjection._33, "invProjection._33");
	SetParameter(_sceneCB.invProjection._34, "invProjection._34");
	SetParameter(_sceneCB.invProjection._41, "invProjection._41");
	SetParameter(_sceneCB.invProjection._42, "invProjection._42");
	SetParameter(_sceneCB.invProjection._43, "invProjection._43");
	SetParameter(_sceneCB.invProjection._44, "invProjection._44");

	SetParameter(_sceneCB.invViewProjection._11, "invViewProjection._11");
	SetParameter(_sceneCB.invViewProjection._12, "invViewProjection._12");
	SetParameter(_sceneCB.invViewProjection._13, "invViewProjection._13");
	SetParameter(_sceneCB.invViewProjection._14, "invViewProjection._14");
	SetParameter(_sceneCB.invViewProjection._21, "invViewProjection._21");
	SetParameter(_sceneCB.invViewProjection._22, "invViewProjection._22");
	SetParameter(_sceneCB.invViewProjection._23, "invViewProjection._23");
	SetParameter(_sceneCB.invViewProjection._24, "invViewProjection._24");
	SetParameter(_sceneCB.invViewProjection._31, "invViewProjection._31");
	SetParameter(_sceneCB.invViewProjection._32, "invViewProjection._32");
	SetParameter(_sceneCB.invViewProjection._33, "invViewProjection._33");
	SetParameter(_sceneCB.invViewProjection._34, "invViewProjection._34");
	SetParameter(_sceneCB.invViewProjection._41, "invViewProjection._41");
	SetParameter(_sceneCB.invViewProjection._42, "invViewProjection._42");
	SetParameter(_sceneCB.invViewProjection._43, "invViewProjection._43");
	SetParameter(_sceneCB.invViewProjection._44, "invViewProjection._44");
}

// シーン定数バッファの更新
void ConstantBufferManager::UpdateSceneCB(SceneConstantBuffer* buffer)
{
	if (buffer != nullptr)
	{
		this->_sceneCB.viewProjection = buffer->viewProjection;
		this->_sceneCB.lightDirection = buffer->lightDirection;
		this->_sceneCB.lightColor = buffer->lightColor;
		this->_sceneCB.cameraPosition = buffer->cameraPosition;
		this->_sceneCB.invProjection = buffer->invProjection;
		this->_sceneCB.invView = buffer->invView;
		this->_sceneCB.invViewProjection = buffer->invViewProjection;
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

	SetParameter(_lightCB.ambientColor.x, "ambientColor.x");
	SetParameter(_lightCB.ambientColor.y, "ambientColor.y");
	SetParameter(_lightCB.ambientColor.z, "ambientColor.z");
	SetParameter(_lightCB.ambientColor.w, "ambientColor.w");

	SetParameter(_lightCB.directionalLightDirection.x, "directionalLightDirection.x");
	SetParameter(_lightCB.directionalLightDirection.y, "directionalLightDirection.y");
	SetParameter(_lightCB.directionalLightDirection.z, "directionalLightDirection.z");
	SetParameter(_lightCB.directionalLightDirection.w, "directionalLightDirection.w");

	SetParameter(_lightCB.directionalLightColor.x, "directionalLightColor.x");
	SetParameter(_lightCB.directionalLightColor.y, "directionalLightColor.y");
	SetParameter(_lightCB.directionalLightColor.z, "directionalLightColor.z");
	SetParameter(_lightCB.directionalLightColor.w, "directionalLightColor.w");

	for (int i = 0; i < _countof(_lightCB.pointLights); i++)
	{
		std::string frontStr = "pointLights[";
		frontStr += std::to_string(i);
		frontStr += "].";
		SetParameter(_lightCB.pointLights[i].position.x, frontStr + "position.x");
		SetParameter(_lightCB.pointLights[i].position.y, frontStr + "position.y");
		SetParameter(_lightCB.pointLights[i].position.z, frontStr + "position.z");
		SetParameter(_lightCB.pointLights[i].position.w, frontStr + "position.w");

		SetParameter(_lightCB.pointLights[i].color.x, frontStr + "color.x");
		SetParameter(_lightCB.pointLights[i].color.y, frontStr + "color.y");
		SetParameter(_lightCB.pointLights[i].color.z, frontStr + "color.z");
		SetParameter(_lightCB.pointLights[i].color.w, frontStr + "color.w");

		SetParameter(_lightCB.pointLights[i].range, frontStr + "range");
		{
			std::string str = frontStr + "isAlive";
			iter = parameter.find(str.c_str());
			if (iter != parameter.end())
				_lightCB.pointLights[i].isAlive = (int)(*iter).second;
		}
	}
}

// ライト定数バッファの更新
void ConstantBufferManager::UpdateLightCB(LightConstantBuffer* buffer)
{
	if (buffer != nullptr)
	{
		this->_lightCB.ambientColor = buffer->ambientColor;
		this->_lightCB.directionalLightDirection = buffer->directionalLightDirection;
		this->_lightCB.directionalLightColor = buffer->directionalLightColor;
		for (int i = 0; i < _countof(_lightCB.pointLights); ++i)
		{
			this->_lightCB.pointLights[i] = buffer->pointLights[i];
		}
	}
}
