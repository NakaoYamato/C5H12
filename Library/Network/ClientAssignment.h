#pragma once

#include <NetworkDefine.h>

#include <string>
#include <vector>

/// <summary>
/// クライアント
/// </summary>
class ClientAssignment
{
public:
	ClientAssignment() {}
	~ClientAssignment() {}

	/// <summary>
	/// 開始処理
	/// </summary>
	void Execute(const char* ipAddress = "127.0.0.1");

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Exit();

#pragma region コールバック関数
	/// <summary>
	/// サーバーと接続時に呼ばれる関数
	/// </summary>
	/// <param name="connection"></param>
	/// <param name="connectionData">自身(ClientAssignment)のポインタ</param>
	static void Connect(ENLConnection connection, void* connectionData);

	/// <summary>
	/// サーバーと切断されたときに呼ばれる関数
	/// </summary>
	/// <param name="connection"></param>
	/// <param name="connectionData">自身(ClientAssignment)のポインタ</param>
	static void Disconnect(ENLConnection connection, void* connectionData);

	/// <summary>
	/// サーバーからデータが送られたときに呼ばれる関数
	/// </summary>
	/// <param name="connection"></param>
	/// <param name="connectionData">自身(ClientAssignment)のポインタ</param>
	/// <param name="payloadType">送られたデータタイプ</param>
	/// <param name="payload">データのポインタ</param>
	/// <param name="payloadLen">データの長さ</param>
	static void ReadRecord(ENLConnection connection, void* connectionData, uint16_t payloadType, const void* payload, uint32_t payloadLen);
#pragma endregion

	// GUI表示
	void DrawGui();
private:
	ENLConnection connection = 0;

	// デバッグ用
	bool _drawGui = false;
	std::vector<std::string> _logs;
};