#pragma once

#include "../../Library/Particle/ParticleCanvas.h"
#include "../../Library/Particle/ParticleDefine.h"
#include "../../Library/Renderer/ParticleRenderer.h"

#include "../../Library/Exporter/Exporter.h"
class ParticleEmiter
{
public:
	// 処理タイプ
	enum class ProcessType
	{
		None,			// 処理なし
		Random,			// ランダム処理
		RandomBias,
		RandomNormal,
		Linear,			// 線形処理
	};

	struct ExtensionInt
	{
		ProcessType processType = ProcessType::None; // 処理タイプ
		int			base		= 0; // 基準値
		int			value0		= 0; // 値0
		int			value1		= 0; // 値1

		int Processing() const;
		void DrawGui(const char* label, const char* const processTypes[], int processTypeCount);
		void Export(const char* label, nlohmann::json* jsonData);
		void Load(const char* label, nlohmann::json& jsonData);
	};
	struct ExtensionFloat
	{
		ProcessType processType = ProcessType::None; // 処理タイプ
		float		base = 0.0f; // 基準値
		float		value0 = 0.0f; // 値0
		float		value1 = 0.0f; // 値1

		float Processing() const;
		void DrawGui(const char* label, const char* const processTypes[], int processTypeCount);
		void Export(const char* label, nlohmann::json* jsonData);
		void Load(const char* label, nlohmann::json& jsonData);
	};
	struct ExtensionVector3
	{
		ProcessType processType = ProcessType::None; // 処理タイプ
		Vector3		base = Vector3::One; // 基準値
		Vector3		value0 = Vector3::One; // 値0
		Vector3		value1 = Vector3::One; // 値1

		Vector3 Processing() const;
		void DrawGui(const char* label, const char* const processTypes[], int processTypeCount);
		void Export(const char* label, nlohmann::json* jsonData);
		void Load(const char* label, nlohmann::json& jsonData);
	};
	struct ExtensionVector4
	{
		ProcessType processType = ProcessType::None; // 処理タイプ
		Vector4		base = Vector4::One; // 基準値
		Vector4		value0 = Vector4::One; // 値0
		Vector4		value1 = Vector4::One; // 値1

		Vector4 Processing() const;
		void DrawGui(const char* label, const char* const processTypes[], int processTypeCount);
		void Export(const char* label, nlohmann::json* jsonData);
		void Load(const char* label, nlohmann::json& jsonData);
	};
public:
	ParticleEmiter() = default;
	~ParticleEmiter() = default;

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="elapsedTime"></param>
	/// <param name="renderer"></param>
	/// <param name="transform"></param>
	void Update(float elapsedTime, ParticleRenderer& renderer, const DirectX::XMFLOAT4X4& transform);
	/// <summary>
	/// パーティクルを生成
	/// </summary>
	/// <param name="renderer"></param>
	/// <param name="transform"></param>
	void Emit(ParticleRenderer& renderer, const DirectX::XMFLOAT4X4& transform);
	/// <summary>
	/// Gui描画
	/// </summary>
	/// <param name="renderer"></param>
	void DrawGui(ParticleRenderer& renderer);

	void Play(ParticleRenderer& renderer, const DirectX::XMFLOAT4X4& transform);
	bool IsPlaying() const { return _playing; }
	void Stop()
	{
		_playing = false;
		_playElapsedTimer = 0.0f;
		_emitIntervalElapsedTimer = 0.0f;
	}

	void Export(std::string filename);
	void Inport(std::string filename);
private:
	// シリアライズファイルパス
	std::string			_serializedFilePath = "./Data/Particle/ParticleEmiter.json";

	// 再生中
	bool		_playing = false;
	// 再生時間
	float		_playingTime = 0.0f;
	// 再生経過時間
	float		_playElapsedTimer = 0.0f;
	// 生成間隔
	float		_emitIntervalTime = 0.0f;
	// 生成経過時間
	float		_emitIntervalElapsedTimer = 0.0f;

#pragma region 書き出す要素
	// テクスチャキー
	std::string			_textureKey;
	// 更新タイプ
	ParticleUpdateType	_updateType = ParticleUpdateType::Default;
	// 描画タイプ
	ParticleRenderType	_renderType = ParticleRenderType::Default;
	//　テクスチャ切り取り番号
	UINT		_texcoordIndex = 0;

	// 再生時間
	ExtensionFloat	_playTime;
	// 生成間隔
	ExtensionFloat	_emitInterval;
	// 生成数
	ExtensionInt	_emitCount;
	// 生存時間
	ExtensionFloat	_lifeTimer;
	// 発生位置
	ExtensionVector3 _position;
	// 開始時の回転
	ExtensionVector3 _startRotation;
	// 終了時の回転
	ExtensionVector3 _endRotation;
	// 開始時の大きさ
	ExtensionVector3 _startScale;
	// 終了時の大きさ
	ExtensionVector3 _endScale;
	// 速度
	ExtensionVector3 _velocity;
	// 加速力
	ExtensionVector3 _acceleration;
	// 初期色
	ExtensionVector4 _startColor;
	// 終了色
	ExtensionVector4 _endColor;
	// テクスチャアニメーション
	ExtensionFloat	_texAnimTime;
#pragma endregion
};