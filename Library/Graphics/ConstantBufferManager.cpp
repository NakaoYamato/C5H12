#include "ConstantBufferManager.h"
#include "../Graphics/GpuResourceManager.h"

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
		DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.camera->GetView());
		DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.camera->GetProjection());
		DirectX::XMStoreFloat4x4(&sceneCB.view, V);
		DirectX::XMStoreFloat4x4(&sceneCB.projection, V);
		DirectX::XMStoreFloat4x4(&sceneCB.viewProjection, V * P);
		DirectX::XMStoreFloat4x4(&sceneCB.invProjection, DirectX::XMMatrixInverse(nullptr, P));
		DirectX::XMStoreFloat4x4(&sceneCB.invView, DirectX::XMMatrixInverse(nullptr, V));
		DirectX::XMStoreFloat4x4(&sceneCB.invViewProjection, DirectX::XMMatrixInverse(nullptr, V * P));
		sceneCB.lightDirection = rc.lightDirection;
		sceneCB.lightColor = rc.lightColor;
		const DirectX::XMFLOAT3& eye = rc.camera->GetEye();
		sceneCB.cameraPosition.x = eye.x;
		sceneCB.cameraPosition.y = eye.y;
		sceneCB.cameraPosition.z = eye.z;

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
