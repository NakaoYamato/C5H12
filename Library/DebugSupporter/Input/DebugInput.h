#pragma once

#include <Windows.h>

using DebugButton = unsigned int;

struct DebugInput
{
	static const DebugButton BTN_F1 = (1 << 0);
	static const DebugButton BTN_F2 = (1 << 1);
	static const DebugButton BTN_F3 = (1 << 2);
	static const DebugButton BTN_F4 = (1 << 3);
	static const DebugButton BTN_F5 = (1 << 4);
	static const DebugButton BTN_F6 = (1 << 5);
	static const DebugButton BTN_F7 = (1 << 6);
	static const DebugButton BTN_F8 = (1 << 7);
	static const DebugButton BTN_F9 = (1 << 8);

	DebugButton		buttonData_ = 0;
};