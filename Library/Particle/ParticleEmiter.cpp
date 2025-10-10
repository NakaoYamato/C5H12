#include "ParticleEmiter.h"

#include "../../Library/HRTrace.h"
#include "../../Library/Graphics/GpuResourceManager.h"

#include "../../Math/Random.h"

#include "../../DebugSupporter/DebugSupporter.h"
#include "../../External/nameof/include/nameof.hpp"
#include "../../External/magic_enum/include/magic_enum/magic_enum.hpp"
#include <imgui.h>

int ParticleEmiter::ExtensionInt::Processing() const
{
	switch (processType)
	{
	case ParticleEmiter::ProcessType::None:
		return base;
	case ParticleEmiter::ProcessType::Random:
		return rand() % (value0)+base;
		break;
	case ParticleEmiter::ProcessType::RandomBias:
		return static_cast<int>(Random::RandBias() * value0) + base;
		break;
	case ParticleEmiter::ProcessType::RandomNormal:
		return static_cast<int>(Random::Rand01() * value0) + base;
		break;
	default:
		return base;
	}
}

void ParticleEmiter::ExtensionInt::DrawGui(const char* label, const char* const processTypes[], int processTypeCount)
{
	if (ImGui::TreeNode(label))
	{
		ImGui::Combo(u8"処理", reinterpret_cast<int*>(&processType), processTypes, processTypeCount);
		ImGui::DragInt(u8"ベース", &base);
		if (ProcessType::Random <= processType && processType <= ProcessType::RandomNormal)
		{
			ImGui::DragInt(u8"ランダム値", &value0);
		}
		ImGui::TreePop();
	}
}

void ParticleEmiter::ExtensionInt::Export(const char* label, nlohmann::json* jsonData)
{
	(*jsonData)[label] = {
		{"processType", processType},
		{"base",		base},
		{"value0",		value0},
		{"value1",		value1}
	};
}

void ParticleEmiter::ExtensionInt::Load(const char* label, nlohmann::json& jsonData)
{
	if (jsonData.contains(label))
	{
		auto& data = jsonData[label];
		processType	= data.value("processType", processType);
		base		= data.value("base", base);
		value0		= data.value("value0", value0);
		value1		= data.value("value1", value1);
	}
}

float ParticleEmiter::ExtensionFloat::Processing() const
{
	switch (processType)
	{
	case ParticleEmiter::ProcessType::None:
		return base;
	case ParticleEmiter::ProcessType::Random:
		return Random::Rand(0.0f, value0) + base;
		break;
	case ParticleEmiter::ProcessType::RandomBias:
		return (Random::RandBias() * value0) + base;
		break;
	case ParticleEmiter::ProcessType::RandomNormal:
		return (Random::Rand01() * value0) + base;
		break;
	default:
		return base;
	}
}

void ParticleEmiter::ExtensionFloat::DrawGui(const char* label, const char* const processTypes[], int processTypeCount)
{
	if (ImGui::TreeNode(label))
	{
		ImGui::Combo(u8"処理", reinterpret_cast<int*>(&processType), processTypes, processTypeCount);
		ImGui::DragFloat(u8"ベース", &base, 0.1f);
		if (ProcessType::Random <= processType && processType <= ProcessType::RandomNormal)
		{
			ImGui::DragFloat(u8"ランダム値", &value0, 0.1f);
		}
		ImGui::TreePop();
	}
}

void ParticleEmiter::ExtensionFloat::Export(const char* label, nlohmann::json* jsonData)
{
	(*jsonData)[label] = {
		{"processType", processType},
		{"base",		base},
		{"value0",		value0},
		{"value1",		value1}
	};
}

void ParticleEmiter::ExtensionFloat::Load(const char* label, nlohmann::json& jsonData)
{
	if (jsonData.contains(label))
	{
		auto& data = jsonData[label];
		processType = data.value("processType", processType);
		base		= data.value("base", base);
		value0		= data.value("value0", value0);
		value1		= data.value("value1", value1);
	}
}

Vector3 ParticleEmiter::ExtensionVector3::Processing() const
{
	switch (processType)
	{
	case ParticleEmiter::ProcessType::None:
		return base;
	case ParticleEmiter::ProcessType::Random:
		return Vector3::Random(Vector3::Zero, value0) + base;
	case ParticleEmiter::ProcessType::RandomBias:
		return Vector3::Multiply(Vector3::RandomBias(), value0) + base;
	case ParticleEmiter::ProcessType::RandomNormal:
		return Vector3::Multiply(Vector3::RandomNormal(), value0) + base;
	default:
		return base;
	}
}

void ParticleEmiter::ExtensionVector3::DrawGui(const char* label, const char* const processTypes[], int processTypeCount)
{
	if (ImGui::TreeNode(label))
	{
		ImGui::Combo(u8"処理", reinterpret_cast<int*>(&processType), processTypes, processTypeCount);
		ImGui::DragFloat3(u8"ベース", &base.x, 0.1f);
		if (ProcessType::Random <= processType && processType <= ProcessType::RandomNormal)
		{
			ImGui::DragFloat3(u8"ランダム値", &value0.x, 0.1f);
		}
		ImGui::TreePop();
	}
}

void ParticleEmiter::ExtensionVector3::Export(const char* label, nlohmann::json* jsonData)
{
	(*jsonData)[label] = {
		{"processType", processType},
		{"base.x",		base.x},
		{"base.y",		base.y},
		{"base.z",		base.z},
		{"value0.x",	value0.x},
		{"value0.y",	value0.y},
		{"value0.z",	value0.z},
		{"value1.x",	value1.x},
		{"value1.y",	value1.y},
		{"value1.z",	value1.z}
	};
}

void ParticleEmiter::ExtensionVector3::Load(const char* label, nlohmann::json& jsonData)
{
	if (jsonData.contains(label))
	{
		auto& data = jsonData[label];
		processType = data.value("processType", processType);
		base.x		= data.value("base.x", base.x);
		base.y		= data.value("base.y", base.y);
		base.z		= data.value("base.z", base.z);
		value0.x	= data.value("value0.x", value0.x);
		value0.y	= data.value("value0.y", value0.y);
		value0.z	= data.value("value0.z", value0.z);
		value1.x	= data.value("value1.x", value1.x);
		value1.y	= data.value("value1.y", value1.y);
		value1.z	= data.value("value1.z", value1.z);
	}
}

Vector4 ParticleEmiter::ExtensionVector4::Processing() const
{
	switch (processType)
	{
	case ParticleEmiter::ProcessType::None:
		return base;
	case ParticleEmiter::ProcessType::Random:
		return Vector4::Random(Vector4::Zero, value0) + base;
		break;
	case ParticleEmiter::ProcessType::RandomBias:
		return Vector4::Multiply(Vector4::RandomBias(), value0) + base;
		break;
	case ParticleEmiter::ProcessType::RandomNormal:
		return Vector4::Multiply(Vector4::RandomNormal(), value0) + base;
		break;
	default:
		return base;
	}
}

void ParticleEmiter::ExtensionVector4::DrawGui(const char* label, const char* const processTypes[], int processTypeCount)
{
	if (ImGui::TreeNode(label))
	{
		ImGui::Combo(u8"処理", reinterpret_cast<int*>(&processType), processTypes, processTypeCount);
		ImGui::ColorEdit4(u8"ベース", &base.x);
		if (ProcessType::Random <= processType && processType <= ProcessType::RandomNormal)
		{
			ImGui::ColorEdit4(u8"ランダム値", &value0.x);
		}
		ImGui::TreePop();
	}
}

void ParticleEmiter::ExtensionVector4::Export(const char* label, nlohmann::json* jsonData)
{
	(*jsonData)[label] = {
		{"processType", processType},
		{"base.x",		base.x},
		{"base.y",		base.y},
		{"base.z",		base.z},
		{"base.w",		base.w},
		{"value0.x",	value0.x},
		{"value0.y",	value0.y},
		{"value0.z",	value0.z},
		{"value0.w",	value0.w},
		{"value1.x",	value1.x},
		{"value1.y",	value1.y},
		{"value1.z",	value1.z},
		{"value1.w",	value1.w}
	};
}

void ParticleEmiter::ExtensionVector4::Load(const char* label, nlohmann::json& jsonData)
{
	if (jsonData.contains(label))
	{
		auto& data = jsonData[label];
		processType = data.value("processType", processType);
		base.x		= data.value("base.x", base.x);
		base.y		= data.value("base.y", base.y);
		base.z		= data.value("base.z", base.z);
		base.w		= data.value("base.w", base.w);
		value0.x	= data.value("value0.x", value0.x);
		value0.y	= data.value("value0.y", value0.y);
		value0.z	= data.value("value0.z", value0.z);
		value0.w	= data.value("value0.w", value0.w);
		value1.x	= data.value("value1.x", value1.x);
		value1.y	= data.value("value1.y", value1.y);
		value1.z	= data.value("value1.z", value1.z);
		value1.w	= data.value("value1.w", value1.w);
	}
}

/// 更新処理
void ParticleEmiter::Update(float elapsedTime, ParticleRenderer& renderer, const DirectX::XMFLOAT4X4& transform)
{
	// 再生中でない場合は何もしない
	if (!_playing)
		return;

	// タイマー更新
	_playElapsedTimer += elapsedTime;
	_emitIntervalElapsedTimer += elapsedTime;

	// 生成間隔タイマーが生成間隔を超えた場合はパーティクルを生成
	if (_emitIntervalElapsedTimer >= _emitIntervalTime)
	{
		// パーティクルを生成
		Emit(renderer, transform);
		// 生成間隔タイマーをリセット
		_emitIntervalElapsedTimer -= _emitIntervalTime;
	}

	// 再生時間タイマーが再生時間を超えた場合は再生を停止
	if (_playElapsedTimer >= _playingTime)
	{
		_playing = false;
		_playElapsedTimer = 0.0f;
	}
}

/// パーティクルを生成
void ParticleEmiter::Emit(ParticleRenderer& renderer, const DirectX::XMFLOAT4X4& transform)
{
	// パーティクルデータを取得
	auto textureData = renderer.GetTextureData(_textureKey);

	// データ設定
	ParticleEmitData data{};
	data.updateType = _updateType;
	data.renderType = _renderType;
	data.texcoordIndex = _texcoordIndex;
	data.texPosition = textureData.texPosition;
	data.texSize = textureData.texSize;
	data.texSplit = textureData.texSplit;
	int emitCount = _emitCount.Processing();
	for (int i = 0; i < emitCount; ++i)
	{
		// 生存時間
		data.timer			= _lifeTimer.Processing();
		// 発生位置
		data.position		= _position.Processing().TransformCoord(transform);
		// 回転(ラジアン)
		data.startRotation	= _startRotation.Processing();
		data.endRotation	= _endRotation.Processing();
		// 大きさ
		data.startScale		= _startScale.Processing();
		data.endScale		= _endScale.Processing();
		// 速度
		data.velocity		= _velocity.Processing().TransformNormal(transform);
		// 加速力
		data.acceleration	= _acceleration.Processing().TransformNormal(transform);
		// 色
		data.startColor		= _startColor.Processing();
		data.endColor		= _endColor.Processing();
		// テクスチャアニメーション
		data.texAnimTime = _texAnimTime.Processing();

		renderer.Emit(data);
	}
}

/// Gui描画
void ParticleEmiter::DrawGui(ParticleRenderer& renderer)
{
	static const char* ProcessTypeNames[] = {
		"None",
		"Random",
		"RandomBias",
		"RandomNormal"
	};

	static std::vector<const char*> ParticleUpdateTypeNames;
	if (ParticleUpdateTypeNames.size() == 0)
	{
		for (auto layer : magic_enum::enum_values<ParticleUpdateType>())
		{
			ParticleUpdateTypeNames.push_back(magic_enum::enum_name(layer).data());
		}
	}
	static std::vector<const char*> ParticleRenderTypeNames;
	if (ParticleRenderTypeNames.size() == 0)
	{
		for (auto layer : magic_enum::enum_values<ParticleRenderType>())
		{
			ParticleRenderTypeNames.push_back(magic_enum::enum_name(layer).data());
		}
	}

	ImGui::DragFloat(u8"再生経過時間", &_playElapsedTimer, 0.01f, 0.0f, _playingTime, u8"%.2f 秒");
	ImGui::DragFloat(u8"生成経過時間", &_emitIntervalElapsedTimer, 0.01f, 0.0f, _emitIntervalTime, u8"%.2f 秒");
	ImGui::Checkbox(u8"再生中", &_playing);

	ImGui::Text(u8"テクスチャキー %s", _textureKey.c_str());
	if (ImGui::TreeNode(u8"テクスチャキー変更"))
	{
		for (auto& [key, textureData] : renderer.GetTextureDatas())
		{
			if (ImGui::RadioButton(key.c_str(), _textureKey == key))
			{
				_textureKey = key;
			}
		}
		ImGui::TreePop();
	}
	ImGui::Combo(u8"更新タイプ", reinterpret_cast<int*>(&_updateType), ParticleUpdateTypeNames.data(), static_cast<int>(ParticleUpdateTypeNames.size()));
	ImGui::Combo(u8"描画タイプ", reinterpret_cast<int*>(&_renderType), ParticleRenderTypeNames.data(), static_cast<int>(ParticleRenderTypeNames.size()));
	ImGui::InputInt(u8"テクスチャ切り取り番号", reinterpret_cast<int*>(&_texcoordIndex));

	_playTime.		DrawGui(u8"再生時間", ProcessTypeNames, _countof(ProcessTypeNames));
	_emitInterval.	DrawGui(u8"生成間隔", ProcessTypeNames, _countof(ProcessTypeNames));
	_emitCount.		DrawGui(u8"生成数", ProcessTypeNames, _countof(ProcessTypeNames));
	_lifeTimer.		DrawGui(u8"生存時間", ProcessTypeNames, _countof(ProcessTypeNames));
	_position.		DrawGui(u8"発生位置", ProcessTypeNames, _countof(ProcessTypeNames));
	_startRotation.	DrawGui(u8"初期回転", ProcessTypeNames, _countof(ProcessTypeNames));
	_endRotation.	DrawGui(u8"終了回転", ProcessTypeNames, _countof(ProcessTypeNames));
	_startScale.	DrawGui(u8"初期大きさ", ProcessTypeNames, _countof(ProcessTypeNames));
	_endScale.		DrawGui(u8"終了大きさ", ProcessTypeNames, _countof(ProcessTypeNames));
	_velocity.		DrawGui(u8"速度", ProcessTypeNames, _countof(ProcessTypeNames));
	_acceleration.	DrawGui(u8"加速力", ProcessTypeNames, _countof(ProcessTypeNames));
	_startColor.	DrawGui(u8"初期色", ProcessTypeNames, _countof(ProcessTypeNames));
	_endColor.		DrawGui(u8"終了色", ProcessTypeNames, _countof(ProcessTypeNames));
	_texAnimTime.	DrawGui(u8"テクスチャアニメーション時間", ProcessTypeNames, _countof(ProcessTypeNames));

	ImGui::InputText(u8"シリアライズファイルパス", &_serializedFilePath);
	if (ImGui::Button(u8"書き出し"))
	{
		// ダイアログを開く
		char filename[256] = { 0 };
		static const char* filter = "Particle Files(*.json)\0*.json;\0All Files(*.*)\0*.*;\0\0";
		Debug::Dialog::DialogResult result = Debug::Dialog::SaveFileName(
			filename,
			sizeof(filename),
			filter, 
			nullptr,
			"json");
		// ファイルを選択したら
		if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
		{
			_serializedFilePath = filename;
			Export(filename);
		}
	}
	if (ImGui::Button(u8"読み込み"))
	{
		// ダイアログを開く
		std::string filepath;
		std::string currentDirectory;
		static const char* filter = "Particle File(*.json)\0*.json;\0\0";
		Debug::Dialog::DialogResult result = Debug::Dialog::OpenFileName(filepath, currentDirectory, filter); 
		// ファイルを選択したら
		if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
		{
			_serializedFilePath = filepath;
			Inport(_serializedFilePath);
		}
	}
}

void ParticleEmiter::Play(ParticleRenderer& renderer, const DirectX::XMFLOAT4X4& transform)
{
	// パーティクルを生成
	Emit(renderer, transform);
	_playing = true;
	_playingTime = _playTime.Processing();
	_emitIntervalTime = _emitInterval.Processing();
	_playElapsedTimer = 0.0f;
	_emitIntervalElapsedTimer = 0.0f;
}

void ParticleEmiter::Export(std::string filename)
{
	nlohmann::json jsonData;
	{
		jsonData["textureKey"] = _textureKey;
		jsonData["updateType"] = _updateType;
		jsonData["renderType"] = _renderType;
		jsonData["texcoordIndex"] = _texcoordIndex;

		_playTime.Export("playTime", &jsonData);

		_emitInterval.Export("emitInterval", &jsonData);

		_emitCount.Export("emitCount", &jsonData);

		_lifeTimer.Export("lifeTimer", &jsonData);

		_position.Export("position", &jsonData);

		_startRotation.Export("startRotation", &jsonData);
		_endRotation.Export("endRotation", &jsonData);

		_startScale.Export("startScale", &jsonData);
		_endScale.Export("endScale", &jsonData);

		_velocity.Export("velocity", &jsonData);

		_acceleration.Export("acceleration", &jsonData);

		_startColor.Export("startColor", &jsonData);

		_endColor.Export("endColor", &jsonData);

		_texAnimTime.Export("texAnimTime", &jsonData);
	}
	Exporter::SaveJsonFile(filename, jsonData);
}

void ParticleEmiter::Inport(std::string filename)
{
	nlohmann::json jsonData;
	if (!Exporter::LoadJsonFile(filename, &jsonData))
	{
		return;
	}
	_textureKey = jsonData.value("textureKey", _textureKey);
	_updateType = jsonData.value("updateType", _updateType);
	_renderType = jsonData.value("renderType", _renderType);
	_texcoordIndex = jsonData.value("texcoordIndex", _texcoordIndex);

	_playTime.Load("playTime", jsonData);
	_emitInterval.Load("emitInterval", jsonData);
	_emitCount.Load("emitCount", jsonData);
	_lifeTimer.Load("lifeTimer", jsonData);
	_position.Load("position", jsonData);
	_startRotation.Load("startRotation", jsonData);
	_endRotation.Load("endRotation", jsonData);
	_startScale.Load("startScale", jsonData);
	_endScale.Load("endScale", jsonData);
	_velocity.Load("velocity", jsonData);
	_acceleration.Load("acceleration", jsonData);
	_startColor.Load("startColor", jsonData);
	_endColor.Load("endColor", jsonData);
	_texAnimTime.Load("texAnimTime", jsonData);

	_serializedFilePath = filename;
}
