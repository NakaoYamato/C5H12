#include "DirectInput.h"

#include "../../HRTrace.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

# include<assert.h>
#include <windows.h>
#include <iostream>
#include <wbemidl.h>
#include <oleauto.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#define SAFE_RELEASE(p) { if (p != nullptr) { p->Release(); p = nullptr; } }
// XInput デバイスかどうかを判定する関数
// https://learn.microsoft.com/ja-jp/previous-versions/direct-x/bb173051(v=vs.85)?redirectedfrom=MSDN
BOOL IsXInputDevice(LPGUID pGuid)
{
    class BString {
    public:
        BSTR    m_bstr;
        BString(TCHAR* pStr) { if ((m_bstr = SysAllocString((OLECHAR*)pStr)) == NULL) throw - 1; }
        ~BString() { if (m_bstr != NULL) SysFreeString(m_bstr); }
        operator BSTR& () { return m_bstr; }
    };

    BOOL                    bIsXinput = FALSE;
    HRESULT                 hr;

    // CoInit if needed
    hr = CoInitialize(NULL);
    BOOL bCleanupCOM = SUCCEEDED(hr);

    // Create WMI
    IWbemLocator* pIWbemLocator = NULL;
    hr = CoCreateInstance(__uuidof(WbemLocator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IWbemLocator), (LPVOID*)&pIWbemLocator);
    if (SUCCEEDED(hr) && pIWbemLocator != NULL) {

        // Connect to WMI
        BString    bstrNamespace((TCHAR*)L"\\\\.\\root\\cimv2");
        IWbemServices* pIWbemServices = NULL;
        hr = pIWbemLocator->ConnectServer(bstrNamespace, NULL, NULL, 0L, 0L, NULL, NULL, &pIWbemServices);
        if (SUCCEEDED(hr) && pIWbemServices != NULL) {

            // Switch security level to IMPERSONATE.
            BString bstrClassName((TCHAR*)L"Win32_PNPEntity");
            IEnumWbemClassObject* pEnumDevices = NULL;
            hr = CoSetProxyBlanket(pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
            hr = pIWbemServices->CreateInstanceEnum(bstrClassName, 0, NULL, &pEnumDevices);
            if (SUCCEEDED(hr) && pEnumDevices != NULL) {

                DWORD    dwDevIdx{}, dwReturned{};
                DWORD    dwPid{}, dwVid{};
                WCHAR* strVid{}, * strPid{};
                VARIANT var{};
                BString bstrDeviceID((TCHAR*)L"DeviceID");
                IWbemClassObject* pDevices[20] = { 0 };

                // Loop over all devices
                for (; !bIsXinput;) {

                    // Get 20 at a time
                    dwDevIdx = dwReturned = 0;
                    hr = pEnumDevices->Next(10000, 20, pDevices, &dwReturned);
                    if (FAILED(hr) || dwReturned == 0) break;

                    for (dwDevIdx = 0; dwDevIdx < dwReturned; dwDevIdx++) {

                        // For each device, get its device ID
                        hr = pDevices[dwDevIdx]->Get(bstrDeviceID, 0L, &var, NULL, NULL);
                        if (SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != NULL) {

                            // Check if the device ID contains "IG_".  If it does, then it's an XInput device
                            // This information can not be found from DirectInput
                            if (wcsstr(var.bstrVal, L"IG_")) {

                                // If it does, then get the VID/PID from var.bstrVal
                                dwVid = dwVid = 0;
                                strVid = wcsstr(var.bstrVal, L"VID_");
                                strPid = wcsstr(var.bstrVal, L"PID_");
                                if (strVid && swscanf_s(strVid, L"VID_%4X", &dwVid) != 1) dwVid = 0;
                                if (strPid && swscanf_s(strPid, L"PID_%4X", &dwPid) != 1) dwPid = 0;

                                // Compare the VID/PID to the DInput device
                                if (MAKELONG(dwVid, dwPid) == pGuid->Data1) {
                                    bIsXinput = TRUE;
                                    break;
                                }
                            }
                        }
                        SAFE_RELEASE(pDevices[dwDevIdx]);
                    }
                }

                for (dwDevIdx = 0; dwDevIdx < 20; dwDevIdx++) SAFE_RELEASE(pDevices[dwDevIdx]);
                SAFE_RELEASE(pEnumDevices);
            }
            SAFE_RELEASE(pIWbemServices);
        }
        SAFE_RELEASE(pIWbemLocator);
    }

    if (bCleanupCOM)        CoUninitialize();
    return bIsXinput;
}
// DirectInputのゲームパッドを検索するためのコールバック関数
BOOL DirectInput::EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)
{
    DirectInput* self = reinterpret_cast<DirectInput*>(pContext);
    // XInput デバイスはスキップ
    if (IsXInputDevice((LPGUID)&pdidInstance->guidProduct)) {
        return DIENUM_CONTINUE;
    }

    HRESULT hr = self->_directInput->CreateDevice(pdidInstance->guidInstance, &self->_gamepad, nullptr);
    if (FAILED(hr)) {
        return DIENUM_CONTINUE;
    }
    return DIENUM_STOP; // 最初に見つけたデバイスで停止

}
DirectInput::DirectInput(HWND hwnd, HINSTANCE instance)
{
    HRESULT hr = S_OK;
	_hwnd = hwnd;

	// 入力値、状態の初期化
    for (int i = 0; i <= DIRECTPAD_BUTTON_L_UP; ++i) {
        _states[i] = FALSE;
    }
    _values[DIRECTPAD_AXIS_LX] = 0.0f;
    _values[DIRECTPAD_AXIS_LY] = 0.0f;
    _values[DIRECTPAD_AXIS_RX] = 0.0f;
    _values[DIRECTPAD_AXIS_RY] = 0.0f;

	// DirectInputの初期化
	hr = DirectInput8Create(instance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&_directInput, nullptr);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    // ゲームパッドを検索
    SearchGamepad();
}

DirectInput::~DirectInput()
{
	// ゲームパッドの解放
	ReleaseGamepad();

    if (_directInput) {
        _directInput->Release();
        _directInput = nullptr;
    }
}

void DirectInput::Update()
{
	HRESULT hr = S_OK;

    // ゲームパッドがないなら処理しない
    if (!_gamepad)
    {
		// ゲームパッドがない場合は入力状態をクリア
        ClearInput();
        return;
    }

	hr = _gamepad->Poll();
	if (FAILED(hr))
	{
		// ポーリングに失敗した場合は入力状態をクリア
        ClearInput();
		// ポーリングに失敗した場合、再取得を試みる
		hr = _gamepad->Acquire();
        return;
	}

    // ゲームパッド状態（X, Y, Z, RZ, ボタン, POVなど）
    DIJOYSTATE2 joystate{};
	hr = _gamepad->GetDeviceState(sizeof(DIJOYSTATE2), &joystate);
    if (FAILED(hr))
    {
		// デバイスの状態取得に失敗した場合は入力状態をクリア
		ClearInput();
        return;
    }

    // ボタン状態の更新
    for (int i = 0; i <= DIRECTPAD_BUTTON_R3; ++i) {
        _states[i] = (joystate.rgbButtons[i] & 0x80) ? TRUE : FALSE;
    }
    // POV（方向パッド）の状態を更新
	// POVは0~35999の角度（100 = 1度）で表現される
    if (joystate.rgdwPOV[0] == 0xFFFFFFFF)
    {
		// POVが無効な場合はすべての方向をFALSEに設定
		_states[DIRECTPAD_BUTTON_L_UP]      = FALSE;
		_states[DIRECTPAD_BUTTON_L_RIGHT]   = FALSE;
		_states[DIRECTPAD_BUTTON_L_DOWN]    = FALSE;
		_states[DIRECTPAD_BUTTON_L_LEFT]    = FALSE;
	}
    else
    {
        _states[DIRECTPAD_BUTTON_L_UP]      = (joystate.rgdwPOV[0] > 27000 + _povDeadZone   || joystate.rgdwPOV[0] < 9000 - _povDeadZone)   ? TRUE : FALSE; // 上
        _states[DIRECTPAD_BUTTON_L_RIGHT]   = (joystate.rgdwPOV[0] > 0 + _povDeadZone       && joystate.rgdwPOV[0] < 18000 - _povDeadZone)  ? TRUE : FALSE; // 右
        _states[DIRECTPAD_BUTTON_L_DOWN]    = (joystate.rgdwPOV[0] > 9000 + _povDeadZone    && joystate.rgdwPOV[0] < 27000 - _povDeadZone)  ? TRUE : FALSE; // 下
        _states[DIRECTPAD_BUTTON_L_LEFT]    = (joystate.rgdwPOV[0] > 18000 + _povDeadZone   && joystate.rgdwPOV[0] < 36000 - _povDeadZone)  ? TRUE : FALSE; // 左
    }

    _values[DIRECTPAD_AXIS_LX] = GetJoystickAxisValue(joystate.lX);
	_values[DIRECTPAD_AXIS_LY] = -GetJoystickAxisValue(joystate.lY);// Y軸は反転するのでマイナスをつける
	// ※ コントローラーによってはlZ,lRzがlRx,lRyの場合がある
    _values[DIRECTPAD_AXIS_RX] = GetJoystickAxisValue(joystate.lZ);
    _values[DIRECTPAD_AXIS_RY] = -GetJoystickAxisValue(joystate.lRz);
	//_values[DIRECTPAD_AXIS_RX] = GetJoystickAxisValue(joystate.lRx) / 32767.0f; // -1.0f ~ 1.0f
	//_values[DIRECTPAD_AXIS_RY] = GetJoystickAxisValue(joystate.lRy) / 32767.0f; // -1.0f ~ 1.0f
}
// ゲームパッドを検索
void DirectInput::SearchGamepad()
{
    // ゲームパッドの解放
    ReleaseGamepad();

    HRESULT hr = S_OK;
    // DI8DEVCLASS_GAMECTRL -> DirectInputゲームパッド
    hr = _directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    if (_gamepad)
    {
        hr = _gamepad->SetDataFormat(&c_dfDIJoystick2);
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

        hr = _gamepad->SetCooperativeLevel(_hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

        // デバイスをポーリング可能にする
        DIPROPDWORD dipdw{};
        dipdw.diph.dwSize = sizeof(DIPROPDWORD);
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        dipdw.diph.dwObj = 0;
        dipdw.diph.dwHow = DIPH_DEVICE;
        dipdw.dwData = DIPROPAXISMODE_ABS;

        _gamepad->SetProperty(DIPROP_AXISMODE, &dipdw.diph);

        _gamepad->Acquire();
    }
}
/// ボタン番号から文字列に変換
const char* DirectInput::ToString(int vKey)
{
    return nullptr;
}
/// 入力情報をクリアする
void DirectInput::ClearInput()
{
	for (auto& state : _states)
	{
		state.second = FALSE; // すべてのボタン状態をFALSEにする
	}
	for (auto& value : _values)
	{
		value.second = 0.0f; // すべての軸値を0にする
	}
}
/// ジョイスティックの軸値をデッドゾーン処理する
float DirectInput::GetJoystickAxisValue(LONG axisValue) const
{
	float value = static_cast<float>(axisValue) / 32767.0f - 1.0f; // -1.0f ~ 1.0f
	if (value > -_joystickDeadZone && value < _joystickDeadZone) {
		return 0.0f; // デッドゾーン内は0にする
	}
	return std::clamp(value, -1.0f, 1.0f);
}
/// ゲームパッド情報を解放する
void DirectInput::ReleaseGamepad()
{
    if (_gamepad) {
        _gamepad->Unacquire();
        _gamepad->Release();
        _gamepad = nullptr;
    }
}
