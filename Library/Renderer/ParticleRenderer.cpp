#include "ParticleRenderer.h"

#include "../../Library/HRTrace.h"
#include "../../Library/Graphics/GpuResourceManager.h"

#include <imgui.h>

/// 初期化
void ParticleRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* dc, UINT particlesCount)
{
	HRESULT hr = S_OK;

	// バイトニックソートの使用上、パーティクル数を2の累乗にしておく
	float fExponent = log2(static_cast<float>(particlesCount));
	int exponent = static_cast<int>(ceilf(fExponent) + 0.5f);
	particlesCount = static_cast<UINT>(pow(2, exponent) + 0.5f);
	particlesCount = max(min(particlesCount, 1 << 27), 1 << 7); // 1Fでの生成制限数

	// パーティクル数をスレッド数に合わせて制限
	_numParticles = ((particlesCount + (NumParticleThread - 1)) / NumParticleThread) * NumParticleThread;
	_numEmitParticles = min(_numParticles, 10000); // 1Fでの生成制限数
	_oneShotInitialize = false;

	// 定数バッファ
	{
		D3D11_BUFFER_DESC desc{};
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		{
			desc.ByteWidth = sizeof(CommonConstants);
			hr = device->CreateBuffer(&desc, nullptr, _commonConstantBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
		{
			desc.ByteWidth = sizeof(BitonicSortConstants);
			hr = device->CreateBuffer(&desc, nullptr, _bitonicSortConstantBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
	}

	// パーティクルバッファ生成
	{
		D3D11_BUFFER_DESC desc{};
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE |
			D3D11_BIND_UNORDERED_ACCESS;
		desc.ByteWidth = sizeof(ParticleData) * _numParticles;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(ParticleData);
		desc.Usage = D3D11_USAGE_DEFAULT;
		hr = device->CreateBuffer(&desc, nullptr, _particleDataBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		hr = device->CreateShaderResourceView(_particleDataBuffer.Get(), nullptr,
			_particleDataSRV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		hr = device->CreateUnorderedAccessView(_particleDataBuffer.Get(), nullptr,
			_particleDataUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// パーティクルの生成/破棄番号をため込むバッファ生成
	{
		D3D11_BUFFER_DESC desc{};
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.ByteWidth = sizeof(UINT) * _numParticles;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(UINT);
		desc.Usage = D3D11_USAGE_DEFAULT;
		hr = device->CreateBuffer(&desc, nullptr, _particleAppendConsumeBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		//	Append/Consumeを利用する場合はビュー側にフラグを立てる
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = _numParticles;
		uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
		hr = device->CreateUnorderedAccessView(_particleAppendConsumeBuffer.Get(),
			&uavDesc, _particleAppendConsumeUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// パーティクルエミット用バッファ作成
	{
		D3D11_BUFFER_DESC desc{};
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.ByteWidth = sizeof(ParticleEmitData) * _numEmitParticles;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(ParticleEmitData);
		desc.Usage = D3D11_USAGE_DEFAULT;
		hr = device->CreateBuffer(&desc, nullptr,
			_particleEmitBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		hr = device->CreateShaderResourceView(_particleEmitBuffer.Get(),
			nullptr, _particleEmitSRV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// パーティクルの更新、描画数の削減のためのバッファ
	{
		D3D11_BUFFER_DESC desc{};
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		desc.ByteWidth = DrawIndirectSize;
		desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS |
			D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		desc.Usage = D3D11_USAGE_DEFAULT;
		D3D11_SUBRESOURCE_DATA initializeData = {};
		std::vector<UINT> initializeBuffer(desc.ByteWidth / sizeof(UINT));
		{
			// 初期値設定
			DrawIndirect* drawData = reinterpret_cast<DrawIndirect*>(initializeBuffer.data() +
				DrawIndirectOffset / sizeof(UINT));
			drawData->vertex_count_per_instance = _numParticles;
			drawData->instance_count = 1;
			drawData->start_vertex_location = 0;
			drawData->start_instance_location = 0;
		}
		initializeData.pSysMem = initializeBuffer.data();
		initializeData.SysMemPitch = desc.ByteWidth;
		hr = device->CreateBuffer(&desc, &initializeData, _indirectDataBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		//	Append/Consumeを利用する場合はビュー側にフラグを立てる
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = desc.ByteWidth / sizeof(UINT);
		uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		hr = device->CreateUnorderedAccessView(_indirectDataBuffer.Get(),
			&uavDesc, _indirectDataUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// パーティクルヘッダーバッファ生成
	{
		D3D11_BUFFER_DESC desc{};
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE |
			D3D11_BIND_UNORDERED_ACCESS;
		desc.ByteWidth = sizeof(ParticleHeader) * _numParticles;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(ParticleHeader);
		desc.Usage = D3D11_USAGE_DEFAULT;
		hr = device->CreateBuffer(&desc, nullptr, _particleHeaderBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		hr = device->CreateShaderResourceView(_particleHeaderBuffer.Get(), nullptr,
			_particleHeaderSRV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		hr = device->CreateUnorderedAccessView(_particleHeaderBuffer.Get(), nullptr,
			_particleHeaderUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// コンピュートシェーダー読み込み
	GpuResourceManager::CreateCsFromCso(device, "./Data/Shader/HLSL/Particle/ComputeParticleInitCS.cso", _initComputeShader.GetAddressOf());
	GpuResourceManager::CreateCsFromCso(device, "./Data/Shader/HLSL/Particle/ComputeParticleEmitCS.cso", _emitComputeShader.GetAddressOf());
	GpuResourceManager::CreateCsFromCso(device, "./Data/Shader/HLSL/Particle/ComputeParticleUpdateCS.cso", _updateComputeShader.GetAddressOf());

	GpuResourceManager::CreateCsFromCso(device, "./Data/Shader/HLSL/Particle/ComputeParticleBeginFrameCS.cso", _beginFrameComputeShader.GetAddressOf());
	GpuResourceManager::CreateCsFromCso(device, "./Data/Shader/HLSL/Particle/ComputeParticleEndFrameCS.cso", _endFrameComputeShader.GetAddressOf());

	GpuResourceManager::CreateCsFromCso(device, "./Data/Shader/HLSL/Particle/Sort/ComputeParticleBitonicSortB2CS.cso", _sortB2ComputeShader.GetAddressOf());
	GpuResourceManager::CreateCsFromCso(device, "./Data/Shader/HLSL/Particle/Sort/ComputeParticleBitonicSortC2CS.cso", _sortC2ComputeShader.GetAddressOf());

	// 描画用情報生成
	GpuResourceManager::CreateVsFromCso(device, "./Data/Shader/HLSL/Particle/Render/ComputeParticleRenderVS.cso", _vertexShader.GetAddressOf(),	nullptr, nullptr, 0);
	GpuResourceManager::CreateGsFromCso(device, "./Data/Shader/HLSL/Particle/Render/ComputeParticleRenderGS.cso", _geometryShader.GetAddressOf());
	GpuResourceManager::CreatePsFromCso(device, "./Data/Shader/HLSL/Particle/Render/ComputeParticleRenderPS.cso", _pixelShader.GetAddressOf());

	// キャンバス作成
	_particleCanvas = std::make_unique<ParticleCanvas>();

	// 画像読み込み
	RegisterTextureData(dc, "Breath", L"./Data/Texture/Particle/AdobeStock_255896219.png", { 3,2 });
	RegisterTextureData(dc, "Test", L"./Data/Texture/Particle/particle256x256.png", { 4,4 });
	RegisterTextureData(dc, "Breath2", L"./Data/Texture/Particle/DM0N4p2f6nXdX9v1753233487_1753233552.png", { 3,2 });
	RegisterTextureData(dc, "Breath3", L"./Data/Texture/Particle/DM0N4p2f6nXdX9v1753233487_1753234506.png", { 3,2 });
	RegisterTextureData(dc, "Smoke", L"./Data/Texture/Particle/Smoke.png", { 2,2 });
}

/// パーティクル生成
void ParticleRenderer::Emit(const ParticleEmitData& data)
{
	// 生成数が制限を超えている場合は何もしない
	if (_emitParticles.size() >= _numEmitParticles)
		return;

	_emitParticles.emplace_back(data);
}

/// 更新処理
void ParticleRenderer::Update(ID3D11DeviceContext* dc, float elapsedTime)
{
	// 定数バッファ設定
	{
		dc->CSSetConstantBuffers(10, 1, _commonConstantBuffer.GetAddressOf());

		// 定数バッファ更新
		CommonConstants constant{};
		constant.elapsedTime = elapsedTime;
		constant.canvasSize.x = static_cast<float>(ParticleCanvas::CanvasWidth);
		constant.canvasSize.y = static_cast<float>(ParticleCanvas::CanvasHeight);
		constant.systemNumParticles = _numParticles;
		constant.totalEmitCount = static_cast<UINT>(_emitParticles.size());
		dc->UpdateSubresource(_commonConstantBuffer.Get(), 0, nullptr, &constant, 0, 0);

		dc->CSSetConstantBuffers(11, 1, _bitonicSortConstantBuffer.GetAddressOf());
	}

	// SRV/UAV設定
	{
		dc->CSSetShaderResources(0, 1, _particleEmitSRV.GetAddressOf());

		ID3D11UnorderedAccessView* uavs[] =
		{
			_particleDataUAV.Get(),
			_particleAppendConsumeUAV.Get(),
			_indirectDataUAV.Get(),
			_particleHeaderUAV.Get(),
		};
		dc->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);
	}

	// 初期化処理
	bool isParticleInit = false;
	if (!_oneShotInitialize)
	{
		// AppendStructuredBufferとConsumeStructuredBufferのカウントをリセット
		UINT counterInit = 0;
		dc->CSSetUnorderedAccessViews(1, 1, _particleAppendConsumeUAV.GetAddressOf(), &counterInit);
		isParticleInit = true;
		_oneShotInitialize = true;
		dc->CSSetShader(_initComputeShader.Get(), nullptr, 0);
		dc->Dispatch(_numParticles / NumParticleThread, 1, 1);
	}

	// フレーム開始時の処理
	{
		// 現在フレームでのパーティクル総数を算出
		// それに合わせて各種設定
		dc->CSSetShader(_beginFrameComputeShader.Get(), nullptr, 0);
		dc->Dispatch(1, 1, 1);
	}

	// エミット処理
	if (!isParticleInit && !_emitParticles.empty())
	{
		// エミットバッファ更新
		D3D11_BOX writeBox = {};
		writeBox.left = 0;
		writeBox.right = static_cast<UINT>(_emitParticles.size() * sizeof(ParticleEmitData));
		writeBox.top = 0;
		writeBox.bottom = 1;
		writeBox.front = 0;
		writeBox.back = 1;
		dc->UpdateSubresource(
			_particleEmitBuffer.Get(),
			0,
			&writeBox,
			_emitParticles.data(),
			static_cast<UINT>(_emitParticles.size() * sizeof(ParticleEmitData)),
			0);
		dc->CSSetShader(_emitComputeShader.Get(), nullptr, 0);
		dc->DispatchIndirect(_indirectDataBuffer.Get(), EmitDispatchIndirectOffset);
		_emitParticles.clear();
	}

	// 更新処理
	{
		dc->CSSetShader(_updateComputeShader.Get(), nullptr, 0);
		dc->DispatchIndirect(_indirectDataBuffer.Get(), UpdateDispatchIndirectOffset);
	}

	// ソート処理
	{
		// バイトニックソート		
		//	https://www.bealto.com/gpu-sorting_parallel-bitonic-1.html
		float fExponent = log2(static_cast<float>(_numParticles));
		UINT exponent = static_cast<UINT>(ceilf(fExponent) + 0.5f);
		for (UINT i = 0; i < exponent; ++i)
		{
			dc->CSSetShader(_sortB2ComputeShader.Get(), nullptr, 0);
			UINT increment = 1 << i;
			for (UINT j = 0; j < i + 1; ++j)
			{
				BitonicSortConstants constant{};
				constant.increment = increment;
				constant.direction = 2 << i;
				dc->UpdateSubresource(_bitonicSortConstantBuffer.Get(),
					0,
					nullptr,
					&constant,
					0,
					0);
				if (increment <= BitonicSortC2Thread)
				{
					dc->CSSetShader(_sortC2ComputeShader.Get(), nullptr, 0);
					dc->Dispatch(_numParticles / 2 / BitonicSortC2Thread, 1, 1);

					break;
				}

				dc->Dispatch(_numParticles / 2 / BitonicSortB2Thread, 1, 1);
				increment /= 2;
			}
		}
	}

	// フレーム終了時の処理
	{
		dc->CSSetShader(_endFrameComputeShader.Get(), nullptr, 0);
		dc->Dispatch(1, 1, 1);
	}

	// UAV設定
	{
		ID3D11UnorderedAccessView* uavs[] = { nullptr,nullptr,nullptr,nullptr };
		dc->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);
	}
}

/// 描画処理
void ParticleRenderer::Render(ID3D11DeviceContext* dc)
{
	// 点描画設定
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// シェーダー設定
	dc->VSSetShader(_vertexShader.Get(), nullptr, 0);
	dc->GSSetShader(_geometryShader.Get(), nullptr, 0);
	dc->PSSetShader(_pixelShader.Get(), nullptr, 0);

	// 入力レイアウト設定
	dc->IASetInputLayout(nullptr);

	// リソース設定
	dc->PSSetShaderResources(TextureSRVStartNum, 1, _particleCanvas->GetColorSRV().GetAddressOf());
	dc->GSSetShaderResources(0, 1, _particleDataSRV.GetAddressOf());
	dc->GSSetShaderResources(1, 1, _particleHeaderSRV.GetAddressOf());

	// バッファクリア
	ID3D11Buffer* clearBuffer[] = { nullptr };
	UINT strides[] = { 0 };
	UINT offsets[] = { 0 };
	dc->IASetVertexBuffers(0, 1, clearBuffer, strides, offsets);
	dc->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

	// パーティクル情報分画像コール
	dc->DrawInstancedIndirect(_indirectDataBuffer.Get(), DrawIndirectOffset);

	// シェーダー無効化
	dc->VSSetShader(nullptr, nullptr, 0);
	dc->GSSetShader(nullptr, nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);

	// リソースクリア
	ID3D11ShaderResourceView* nullSRV[] = { nullptr };
	dc->PSSetShaderResources(TextureSRVStartNum, 1, nullSRV);
	dc->GSSetShaderResources(0, 1, nullSRV);
	dc->GSSetShaderResources(1, 1, nullSRV);
}

/// GUI描画
void ParticleRenderer::DrawGui()
{
	// メニューバー
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu(u8"デバッグ"))
		{
			ImGui::Checkbox(u8"パーティクル", &_debugDraw);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (_debugDraw)
	{
		if (ImGui::Begin(u8"パーティクル"))
		{
			_particleCanvas->DrawGui();
		}
		ImGui::End();
	}
}

/// テクスチャの登録
ParticleCanvas::TextureData ParticleRenderer::RegisterTextureData(
	ID3D11DeviceContext* dc,
	const std::string& key, 
	const std::wstring& filepath,
	DirectX::XMUINT2	split)
{
	// すでに登録されている場合はそのまま返す
	auto it = _textureDatas.find(key);
	if (it != _textureDatas.end())
	{
		return it->second;
	}
	// テクスチャのロード
	_textureDatas[key] = _particleCanvas->Load(dc, filepath.c_str(), split);
	// ロードしたテクスチャ情報を返す
	return _textureDatas[key];
}

/// テクスチャデータの取得
ParticleCanvas::TextureData ParticleRenderer::GetTextureData(const std::string& key) const
{
	// キーが存在しない場合はエラー
	if (_textureDatas.find(key) == _textureDatas.end())
	{
		_ASSERT_EXPR(false, L"テクスチャが登録されていません");
		return ParticleCanvas::TextureData();
	}

	return _textureDatas.at(key);
}
