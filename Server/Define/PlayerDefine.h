#pragma once

#include <string>

namespace Network
{
	/// <summary>
	/// プレイヤーのメインステート
	/// </summary>
	enum class PlayerMainStates : int
	{
		Idle,
		Run,
		Sprint,
		Evade,
		Attack1,
		Guard,
		Hit,
		HitKnockDown,
		GuardHit,
		GuardBreak,
		Death,

		None,
		MaxState,
	};

	/// <summary>
	/// プレイヤーのサブステート
	/// </summary>
	enum class PlayerSubStates : int
	{
		SprintStart,
		Sprinting,
		SprintAttack,

		ComboAttack1,
		ComboAttack2,
		ComboAttack3,
		ComboAttack4,

		GuardStart,
		Guarding,

		None,
		MaxSubState,
	};

	/// <summary>
	/// メインステートの名前を取得
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	inline const char* GetPlayerMainStateName(PlayerMainStates state)
	{
		switch (state)
		{
		case PlayerMainStates::Idle:
			return "Idle";
		case PlayerMainStates::Run:
			return "Run";
		case PlayerMainStates::Sprint:
			return "Sprint";
		case PlayerMainStates::Evade:
			return "Evade";
		case PlayerMainStates::Attack1:
			return "Attack1";
		case PlayerMainStates::Guard:
			return "Guard";
		case PlayerMainStates::Hit:
			return "Hit";
		case PlayerMainStates::HitKnockDown:
			return "HitKnockDown";
		case PlayerMainStates::GuardHit:
			return "GuardHit";
		case PlayerMainStates::GuardBreak:
			return "GuardBreak";
		case PlayerMainStates::Death:
			return "Death";
		case PlayerMainStates::None:
			return "";
		case PlayerMainStates::MaxState:
		default:
			return "error";
		}
	}

	/// <summary>
	/// 文字列からメインステートを取得
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	inline PlayerMainStates GetPlayerMainStateFromName(const char* name)
	{
		for (int i = 0; i < static_cast<int>(PlayerMainStates::MaxState); ++i)
		{
			if (strcmp(name, GetPlayerMainStateName(static_cast<PlayerMainStates>(i))) == 0)
				return static_cast<PlayerMainStates>(i);
		}
		return PlayerMainStates::None;
	}

	/// <summary>
	/// プレイヤーのサブステートの名前を取得
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	inline const char* GetPlayerSubStateName(PlayerSubStates state)
	{
		switch (state)
		{
		case PlayerSubStates::SprintStart:
			return "SprintStart";
		case PlayerSubStates::Sprinting:
			return "Sprinting";
		case PlayerSubStates::SprintAttack:
			return "SprintAttack";
		case PlayerSubStates::ComboAttack1:
			return "ComboAttack1";
		case PlayerSubStates::ComboAttack2:
			return "ComboAttack2";
		case PlayerSubStates::ComboAttack3:
			return "ComboAttack3";
		case PlayerSubStates::ComboAttack4:
			return "ComboAttack4";
		case PlayerSubStates::GuardStart:
			return "GuardStart";
		case PlayerSubStates::Guarding:
			return "Guarding";
		case PlayerSubStates::None:
			return "";
		case PlayerSubStates::MaxSubState:
		default:
			return "error";
		}
	}

	/// <summary>
	/// 文字列からサブステートを取得
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	inline PlayerSubStates GetPlayerSubStateFromName(const char* name)
	{
		for (int i = 0; i < static_cast<int>(PlayerSubStates::MaxSubState); ++i)
		{
			if (strcmp(name, GetPlayerSubStateName(static_cast<PlayerSubStates>(i))) == 0)
				return static_cast<PlayerSubStates>(i);
		}
		return PlayerSubStates::None;
	}
}