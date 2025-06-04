#pragma once

#include "../../Library/Graphics/RenderContext.h"
#include "../../Library/Math/Vector.h"

class ParticleRenderer
{
public:
	// パーティクルスレッド数
	static constexpr UINT NumParticleThread = 1024;
	// 描画時のSRVセット番号
	static constexpr UINT TextureSRVStartNum = 30;

	// パーティクル処理タイプ
	enum class ParticleType : UINT
	{
		None = 0,			// 無効
		Billboard = 1,		// ビルボード
	};

	// パーティクル生成用構造体
	struct EmitData
	{
		// 処理タイプ
		ParticleType	type = ParticleType::None;
		// 切り取り座標番号
		UINT	texcoordIndex = 0;
		// 生存時間
		float	timer = 0.0f;
		// テクスチャアニメーションの速度
		float	texAnimTime = 0.0f;

		Vector4 position = { 0,0,0,0 };
		Vector4 rotation = { 0,0,0,0 };
		Vector4 scale = { 1,1,1,0 };

		Vector4 velocity = { 0,0,0,0 };
		Vector4 acceleration = { 0,0,0,0 };

		Vector4 color = { 1,1,1,1 };
	};

	// パーティクル構造体
	struct ParticleData
	{
		// 処理タイプ
		ParticleType	type = ParticleType::None;
		// 切り取り座標番号
		UINT	texcoordIndex = 0;
		// 生存時間
		float	timer = 0.0f;
		// テクスチャアニメーションの速度
		float	texAnimTime = 0.0f;

		Vector4 position = { 0,0,0,0 };
		Vector4 rotation = { 0,0,0,0 };
		Vector4 scale = { 1,1,1,0 };

		Vector4 velocity = { 0,0,0,0 };
		Vector4 acceleration = { 0,0,0,0 };

		Vector4 texcoord = { 0,0,0,0 };
		Vector4 color = { 1,1,1,1 };
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
		DirectX::XMUINT2	textureSplitCount;
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
    void Initialize(ID3D11Device* device, UINT particlesCount = 100000, DirectX::XMUINT2 splitCount = DirectX::XMUINT2(3, 2));
	/// <summary>
	/// パーティクル生成
	/// </summary>
	/// <param name="data"></param>
	void Emit(const EmitData& data);
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

private:

	UINT _numParticles{};
	UINT _numEmitParticles{};
	bool _oneShotInitialize = false;
	DirectX::XMUINT2 _textureSplitCount{};
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _textureSRV;

	std::vector<EmitData>								_emitParticles;
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
};