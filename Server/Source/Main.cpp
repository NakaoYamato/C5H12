#include <stdio.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

#include "ServerAssignment.h"

std::string UTF16ToShiftJIS(const std::wstring& str) {
	static_assert(sizeof(wchar_t) == 2, "this function is windows only");
	const int len = ::WideCharToMultiByte(932/*CP_ACP*/, 0, str.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string re(len * 2, '\0');
	if (!::WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, &re[0], len, nullptr, nullptr)) {
		const auto ec = ::GetLastError();
		switch (ec)
		{
		case ERROR_INSUFFICIENT_BUFFER:
			throw std::runtime_error("in function utf_16_to_shift_jis, WideCharToMultiByte fail. cause: ERROR_INSUFFICIENT_BUFFER"); break;
		case ERROR_INVALID_FLAGS:
			throw std::runtime_error("in function utf_16_to_shift_jis, WideCharToMultiByte fail. cause: ERROR_INVALID_FLAGS"); break;
		case ERROR_INVALID_PARAMETER:
			throw std::runtime_error("in function utf_16_to_shift_jis, WideCharToMultiByte fail. cause: ERROR_INVALID_PARAMETER"); break;
		default:
			throw std::runtime_error("in function utf_16_to_shift_jis, WideCharToMultiByte fail. cause: unknown(" + std::to_string(ec) + ')'); break;
		}
	}
	const std::size_t realLen = std::strlen(re.c_str());
	re.resize(realLen);
	re.shrink_to_fit();
	return re;
}

int main()
{
	// IPv4アドレス取得
	{
		// https://gist.github.com/yoggy/1241986
		// Windowsソケットの初期化
		WSAData d;
		if (WSAStartup(MAKEWORD(2, 2), &d) != 0) {
			assert("error");
		}

		{
			DWORD rv = DWORD(), size = DWORD();
			PIP_ADAPTER_ADDRESSES adapterAddresses = PIP_ADAPTER_ADDRESSES();

			// adapterAddressesに入れるデータのサイズを取得
			rv = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &size);
			if (rv != ERROR_BUFFER_OVERFLOW) {
				fprintf(stderr, "GetAdaptersAddresses() failed...");
				assert("error");
			}
			// データのサイズ分だけメモリ確保
			adapterAddresses = (PIP_ADAPTER_ADDRESSES)malloc(size);

			// IPv4 IPv6 のアドレス確保
			rv = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, adapterAddresses, &size);
			if (rv != ERROR_SUCCESS) {
				fprintf(stderr, "GetAdaptersAddresses() failed...");
				free(adapterAddresses);
				assert("error");
			}

			PIP_ADAPTER_ADDRESSES pAA;
			pAA = adapterAddresses;
			while (pAA != NULL)
			{
				// Unicode(UTF-16) から ShiftJIS に変換
				std::string adapterName = UTF16ToShiftJIS(pAA->FriendlyName);// アダプタ名
				int adapterIndex = pAA->IfIndex;
				std::cout << "アドレス名:" << adapterName << std::endl;
				std::cout << "アドレス番号:" << adapterIndex << std::endl;

				// ユニキャストアドレスリストの先頭アドレスをセット
				PIP_ADAPTER_UNICAST_ADDRESS pUnicast = pAA->FirstUnicastAddress;
				while (pUnicast)
				{
					// ユニキャストIPアドレスを列挙
					sockaddr* pAddr = pUnicast->Address.lpSockaddr;
					if (pAddr->sa_family == AF_INET)            // IPv4
					{
						char szAddr[NI_MAXHOST];
						inet_ntop(AF_INET, &((struct sockaddr_in*)pAddr)->sin_addr, szAddr, sizeof(szAddr));
						int length = pUnicast->OnLinkPrefixLength;
						std::cout << "	IPv4 : " << szAddr << std::endl;
						std::cout << "	IPv4 Length : " << length << std::endl;

						//if (strcmp(adapterName.c_str(), "Wi-Fi") == 0)
						//{
						//	
						//}
					}
					else if (pAddr->sa_family == AF_INET6)      // IPv6
					{
						char szAddr[NI_MAXHOST];
						inet_ntop(AF_INET6, &((struct sockaddr_in6*)pAddr)->sin6_addr, szAddr, sizeof(szAddr));
						int length = pUnicast->OnLinkPrefixLength;
						std::cout << "	IPv6 : " << szAddr << std::endl;
						std::cout << "	IPv6 Length : " << length << std::endl;
					}

					pUnicast = pUnicast->Next;
				}

				pAA = pAA->Next;
			}

			// メモリ開放
			free(adapterAddresses);
		}

		{
			// Windowsソケットの終了化
			WSACleanup();
		}
	}

	bool loop = true;
	do
	{
		std::cout << "\"exit\"でサーバー再起" << std::endl;

		ServerAssignment server = ServerAssignment();
		server.Execute();
	} while (loop);
}