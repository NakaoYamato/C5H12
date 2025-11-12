#pragma once

#include "../../Library/Particle/ParticleDefine.h"
#include "../../Library/2D/Canvas.h"

class ParticleRenderer
{
public:
	// パーティクルスレッド数
	static constexpr UINT NumParticleThread = 1024;
	// 描画時のSRVセット番号
	static constexpr UINT TextureSRVStartNum = 30;

	// GPU上のパーティクル構造体
	struct ParticleData
	{
		// 処理タイプ
		ParticleUpdateType	updateType = ParticleUpdateType::Default;
		// 描画タイプ
		ParticleRenderType	renderType = ParticleRenderType::Default;
		// 生存時間
		float	lifeTime = 0.0f;
		// 経過時間
		float	elapsedTime = 0.0f;

		// 発生位置
		Vector4 position = Vector4::Zero;
		// 開始時の回転(ラジアン)
		Vector4 startRotation = Vector4::Zero;
		// 終了時の回転(ラジアン)
		Vector4 endRotation = Vector4::Zero;
		// 現在の回転(ラジアン)
		Vector4 rotation = Vector4::Zero;
		// 開始時の大きさ
		Vector4 startScale = Vector4::One;
		// 終了時の大きさ
		Vector4 endScale = Vector4::One;
		// 現在の大きさ
		Vector4 scale = Vector4::One;
		// 速度
		Vector4 velocity = Vector4::Zero;
		// 加速力
		Vector4 acceleration = Vector4::Zero;
		// 初期色
		Vector4 startColor = Vector4::White;
		// 終了色
		Vector4 endColor = Vector4::White;
		// 現在の色
		Vector4 color = Vector4::White;

		// テクスチャ座標(uv)
		Vector4 texcoord = Vector4::Zero;
		// テクスチャ座標
		Vector2 texPosition = Vector2::Zero;
		// テクスチャの大きさ
		Vector2 texSize = Vector2::One;
		// テクスチャの分割数
		DirectX::XMUINT2 texSplit = DirectX::XMUINT2();
		// テクスチャ切り取り番号
		UINT	texcoordIndex = 0;
		// テクスチャアニメーションの速度
		float	texAnimTime = 0.0f;
	};

	// パーティクルヘッダー構造体
	struct ParticleHeader
	{
		UINT	alive;		// 生存フラグ
		UINT	particleIndex;	// パーティクル番号
		float	depth;	// 深度
		UINT	dummy;
	};

	// 汎用情報定義
	struct CommonConstants
	{
		float				elapsedTime;
		Vector2				canvasSize;
		UINT				systemNumParticles;

		UINT				totalEmitCount;
		UINT				commonDummy[3];
	};

	// バイトニックソート情報定義
	struct BitonicSortConstants
	{
		UINT increment;
		UINT direction;
		UINT dummy[2];
	};
	static constexpr UINT BitonicSortB2Thread = 256;
	static constexpr UINT BitonicSortC2Thread = 512;

	// DispatchIndirect用構造体
	using DispatchIndirect = DirectX::XMUINT3;

	//	00バイト目：現在のパーティクル総数
	//	04バイト目：1F前のパーティクル総数
	//	08バイト目：パーティクル破棄数
	//	12バイト目：パーティクル生成用DispatchIndirect情報
	static	constexpr	UINT	NumCurrentParticleOffset	= 0;
	static	constexpr	UINT	NumPreviousParticleOffset	= NumCurrentParticleOffset + sizeof(UINT);
	static	constexpr	UINT	NumDeadParticleOffset		= NumPreviousParticleOffset + sizeof(UINT);
	static	constexpr	UINT	EmitDispatchIndirectOffset	= NumDeadParticleOffset + sizeof(UINT);

	// DrawInstanced用DrawIndirect用構造体
	struct DrawIndirect
	{
		UINT vertex_count_per_instance;
		UINT instance_count;
		UINT start_vertex_location;
		UINT start_instance_location;
	};
	//	24バイト目：パーティクル更新用DispatchIndirect情報
	//	36バイト目：パーティクル生成時に使用するインデックス(Append/Consumeの代わり)
	//	40バイト目：DrawIndirect情報
	static	constexpr	UINT	UpdateDispatchIndirectOffset	= EmitDispatchIndirectOffset + sizeof(DispatchIndirect);
	static	constexpr	UINT	NumEmitParticleIndexOffset		= UpdateDispatchIndirectOffset + sizeof(DispatchIndirect);
	static	constexpr	UINT	DrawIndirectOffset				= NumEmitParticleIndexOffset + sizeof(UINT);
	static	constexpr	UINT	DrawIndirectSize				= DrawIndirectOffset + sizeof(DrawIndirect);

public:
	ParticleRenderer() = default;
	~ParticleRenderer() = default;
    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="device"></param>
    /// <param name="particlesCount"></param>
    void Initialize(ID3D11Device* device, ID3D11DeviceContext* dc, UINT particlesCount = 100000);
	/// <summary>
	/// パーティクル生成
	/// </summary>
	/// <param name="data"></param>
	void Emit(const ParticleEmitData& data);
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="context"></param>
	/// <param name="elapsedTime"></param>
	void Update(ID3D11DeviceContext* dc, float elapsedTime);
	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="rc"></param>
	void Render(ID3D11DeviceContext* dc);
	/// <summary>
	/// GUI描画
	/// </summary>
	void DrawGui(ID3D11Device* device,
		ID3D11DeviceContext* dc);

	/// <summary>
	/// テクスチャデータの登録
	/// </summary>
	/// <param name="device"></param>
	/// <param name="dc"></param>
	/// <param name="key"></param>
	/// <param name="filepath"></param>
	/// <param name="split"></param>
	/// <returns></returns>
	Canvas::TextureData RegisterTextureData(ID3D11Device* device, 
		ID3D11DeviceContext* dc,
		const std::string& key,
		const std::wstring& filepath,
		DirectX::XMUINT2 split = DirectX::XMUINT2(1, 1));
	/// <summary>
	/// テクスチャデータの取得
	/// </summary>
	/// <param name="key"></param>
	/// <returns></returns>
	Canvas::TextureData GetTextureData(const std::string& key) const;
	/// <summary>
	/// テクスチャデータのコンテナ取得
	/// </summary>
	/// <returns></returns>
	std::unordered_map<std::string, Canvas::TextureData>& GetTextureDatas() { return _textureDatas; }
private:
	std::unique_ptr<Canvas> _particleCanvas;
	std::unordered_map<std::string, Canvas::TextureData> _textureDatas;

	UINT _numParticles{};
	UINT _numEmitParticles{};
	bool _oneShotInitialize = false;

	std::vector<ParticleEmitData>						_emitParticles;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				_commonConstantBuffer;

	// パーティクルバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer>				_particleDataBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	_particleDataSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	_particleDataUAV;

	// 未使用パーティクル番号を格納したAppend/Cosumeバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer>				_particleAppendConsumeBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	_particleAppendConsumeUAV;

	// パーティクル生成情報を格納したバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer>				_particleEmitBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	_particleEmitSRV;

	// 各種シェーダー
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>			_initComputeShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>			_emitComputeShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>			_updateComputeShader;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>			_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>		_geometryShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			_pixelShader;

	// DrawIndirectを用いるためRWStrcturedBufferを用いるものに変更
	Microsoft::WRL::ComPtr<ID3D11Buffer>				_indirectDataBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	_indirectDataUAV;

	Microsoft::WRL::ComPtr<ID3D11ComputeShader>			_beginFrameComputeShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>			_endFrameComputeShader;

	// ソート
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>			_sortB2ComputeShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>			_sortC2ComputeShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				_bitonicSortConstantBuffer;

	// パーティクルヘッダーバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer>				_particleHeaderBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	_particleHeaderUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	_particleHeaderSRV;

	// デバッグ表示用
	bool _debugDraw = false;
};