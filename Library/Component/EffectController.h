#pragma once

#include "Component.h"

// 警告の抑制
#pragma warning(push)
#pragma warning(disable:26451)
#pragma warning(disable:26495)
#pragma warning(disable:6385)
#include <Effekseer.h>
#pragma warning(pop)

#include "../../Library/Particle/ParticleEmiter.h"
#include "../../Library/Renderer/ParticleRenderer.h"

class EffectController : public Component
{
public:
#pragma region 各エフェクトデータ
	// エフェクト基底クラス
	class EffectData
	{
	public:
		EffectData() = delete;
		EffectData(Actor* owner) : owner(owner) {}
		virtual ~EffectData() {}

		virtual void Load(std::string filepath) = 0;
		virtual void Play() = 0;
		virtual void Stop() = 0;
		virtual void Update(float elapsedTime) = 0;
		virtual void SetPosition(const Vector3& position) { this->position = position; }
		virtual void SetRotation(const Vector3& rotation) { this->rotation = rotation; }
		virtual void SetScale(const Vector3& scale) { this->scale = scale; }
		virtual const std::string& GetFilePath() const { return filepath; }
		virtual const Vector3& GetPosition() const { return position; }
		virtual const Vector3& GetRotation() const { return rotation; }
		virtual const Vector3& GetScale() const { return scale; }
		virtual void DrawGui();

	protected:
		std::string filepath = "";

		Actor* owner = nullptr;

		Vector3 position = Vector3::Zero;
		Vector3 rotation = Vector3::Zero;
		Vector3 scale = Vector3::One;
	};
	// Effekseerエフェクトデータ
	class EffekseerEffectData : public EffectData
	{
	public:
		EffekseerEffectData(Actor* owner) : EffectData(owner) {}
		~EffekseerEffectData() override {}
		void Load(std::string filepath) override;
		void Play() override;
		void Stop() override;
		void Update(float elapsedTime) override {}
		void SetPosition(const Vector3& position) override;
		void SetRotation(const Vector3& rotation) override;
		void SetScale(const Vector3& scale) override;
		void SetAllColor(const Vector4& color);
		void SetColorFactor(float f);
		void DrawGui() override;
	private:
		Effekseer::EffectRef effekseerEffect;
		Effekseer::Handle handle = -1;
		Vector4 color = Vector4::White;
		float colorFactor = 1.0f;
	};
	// パーティクルエフェクトデータ
	class ParticleEffectData : public EffectData
	{
	public:
		ParticleEffectData(Actor* owner) : EffectData(owner) {}
		~ParticleEffectData() override {}
		void Load(std::string filepath) override;
		void Play() override;
		void Stop() override;
		void Update(float elapsedTime) override;
		void DrawGui() override;
		void SetShowEditorGui(bool show) { showEditorGui = show; }
	private:
		// パーティクルエミッター
		std::unique_ptr<ParticleEmiter> particleEmiter;
		DirectX::XMFLOAT4X4 transform = DirectX::XMFLOAT4X4{ 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
		bool showEditorGui = false;
	};
#pragma endregion

public:
	EffectController() {}
	~EffectController() override {}
	// 名前取得
	const char* GetName() const override { return "EffectController"; }

	// 削除時処
	void OnDelete() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

	// エフェクト再生
	void Play(UINT id);
	// エフェクト再生
	void Play(UINT id, const Vector3& position);
	// エフェクト再生
	void Play(UINT id, const Vector3& position, const Vector3& rotation);
	// エフェクト再生
	void Play(UINT id, const Vector3& position, const Vector3& rotation, const Vector3& scale);
	// エフェクト停止
	void Stop(UINT id);
	// エフェクトデータ取得
	EffectData* GetEffectData(UINT id);
	// Effekseerエフェクト読み込み
	EffekseerEffectData* LoadEffekseerEffect(UINT id, const std::string& filepath);
	// パーティクルエフェクト読み込み
	ParticleEffectData* LoadParticleEffect(UINT id, const std::string& filepath);

private:
	std::unordered_map<UINT, std::unique_ptr<EffectData>> _effectMap;
};